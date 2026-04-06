#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::{
    fs,
    io::Cursor,
    path::PathBuf,
    sync::Mutex,
    thread,
    time::{Duration, SystemTime, UNIX_EPOCH},
};

use reqwest::{
    blocking::{Client, Response as BlockingResponse},
    header::{CONTENT_TYPE, RANGE},
};
use serde::{de::DeserializeOwned, Deserialize, Serialize};
use serde_json::{json, Value};
use tauri::{
    http::{Request, Response, StatusCode},
    AppHandle, Manager, State,
};
use tiny_http::{
    Header as TinyHeader, Method as TinyMethod, Response as TinyResponse, Server as TinyServer,
    StatusCode as TinyStatusCode,
};
use url::Url;

const MEDIA_SERVER_PORT: u16 = 29345;

#[derive(Default)]
struct AppState {
    session: Mutex<Option<JellyfinConfig>>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
struct JellyfinConfig {
    #[serde(rename = "serverUrl")]
    server_url: String,
    username: String,
    #[serde(rename = "accessToken")]
    access_token: String,
    #[serde(rename = "userId")]
    user_id: String,
    #[serde(rename = "deviceName")]
    device_name: String,
    #[serde(rename = "deviceId")]
    device_id: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct ArtistItem {
    Name: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct ImageTags {
    Primary: Option<String>,
}

#[allow(non_snake_case)]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct UserData {
    IsFavorite: bool,
    PlaybackPositionTicks: Option<u64>,
    PlayCount: Option<u64>,
}

#[allow(non_snake_case)]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct Track {
    Id: String,
    Name: String,
    ArtistItems: Vec<ArtistItem>,
    Album: Option<String>,
    RunTimeTicks: Option<u64>,
    ImageTags: Option<ImageTags>,
    UserData: Option<UserData>,
}

#[allow(non_snake_case)]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct Playlist {
    Id: String,
    Name: String,
    ItemCount: Option<u32>,
}

#[allow(non_snake_case)]
#[derive(Debug, Deserialize)]
struct ItemsResponse<T> {
    Items: Vec<T>,
}

#[allow(non_snake_case)]
#[derive(Debug, Deserialize)]
struct AuthResponse {
    User: AuthUser,
    AccessToken: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Deserialize)]
struct AuthUser {
    Id: String,
    Name: String,
}

fn blocking_client() -> Result<Client, String> {
    Client::builder()
        .timeout(Duration::from_secs(20))
        .build()
        .map_err(|error| error.to_string())
}

fn normalize_server_url(server_url: &str) -> String {
    server_url.trim().trim_end_matches('/').to_string()
}

fn device_id() -> String {
    let millis = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_millis();
    format!("myfin-desktop-{millis}")
}

fn auth_header(config: &JellyfinConfig) -> String {
    format!(
        "MediaBrowser Client=\"MyFin Desktop\", Device=\"{}\", DeviceId=\"{}\", Version=\"1.0.0\", Token=\"{}\"",
        config.device_name, config.device_id, config.access_token
    )
}

fn auth_header_without_token(device_name: &str, device_id: &str) -> String {
    format!(
        "MediaBrowser Client=\"MyFin Desktop\", Device=\"{}\", DeviceId=\"{}\", Version=\"1.0.0\"",
        device_name, device_id
    )
}

fn session_file(app: &AppHandle) -> Result<PathBuf, String> {
    let dir = app
        .path()
        .app_local_data_dir()
        .map_err(|error| error.to_string())?;
    fs::create_dir_all(&dir).map_err(|error| error.to_string())?;
    Ok(dir.join("session.json"))
}

fn save_session(app: &AppHandle, config: &JellyfinConfig) -> Result<(), String> {
    let path = session_file(app)?;
    let payload = serde_json::to_vec_pretty(config).map_err(|error| error.to_string())?;
    fs::write(&path, payload).map_err(|error| error.to_string())?;

    #[cfg(unix)]
    {
        use std::os::unix::fs::PermissionsExt;
        fs::set_permissions(&path, fs::Permissions::from_mode(0o600))
            .map_err(|error| error.to_string())?;
    }

    Ok(())
}

fn load_session(app: &AppHandle) -> Result<Option<JellyfinConfig>, String> {
    let path = session_file(app)?;
    if !path.exists() {
        return Ok(None);
    }

    let payload = fs::read_to_string(path).map_err(|error| error.to_string())?;
    let config = serde_json::from_str::<JellyfinConfig>(&payload).map_err(|error| error.to_string())?;
    Ok(Some(config))
}

fn clear_session(app: &AppHandle) -> Result<(), String> {
    let path = session_file(app)?;
    if path.exists() {
        fs::remove_file(path).map_err(|error| error.to_string())?;
    }
    Ok(())
}

fn with_session(state: &State<AppState>) -> Result<JellyfinConfig, String> {
    state
        .session
        .lock()
        .map_err(|_| "Falha ao acessar a sessão atual.".to_string())?
        .clone()
        .ok_or_else(|| "Not authenticated".to_string())
}

fn parse_json_or_error<T: DeserializeOwned>(
    response: BlockingResponse,
    fallback: &str,
) -> Result<T, String> {
    let status = response.status();
    let body = response.text().map_err(|error| error.to_string())?;

    if !status.is_success() {
        if body.contains("<title>Starting Server...</title>")
            || body.contains("Please wait while your application starts")
        {
            return Err("WARMUP_REQUIRED".to_string());
        }

        if body.trim_start().starts_with("<!doctype html") || body.contains("<html") {
            return Err("HTML_RESPONSE".to_string());
        }

        return Err(if body.trim().is_empty() {
            fallback.to_string()
        } else {
            body
        });
    }

    serde_json::from_str(&body).map_err(|error| format!("{fallback}: {error}"))
}

fn get_items<T: DeserializeOwned>(
    state: &State<AppState>,
    endpoint: &str,
    params: &[(&str, String)],
) -> Result<Vec<T>, String> {
    let config = with_session(state)?;
    let client = blocking_client()?;
    let mut url = Url::parse(endpoint).map_err(|error| error.to_string())?;

    {
        let mut query = url.query_pairs_mut();
        for (key, value) in params {
            query.append_pair(key, value);
        }
    }

    let response = client
        .get(url)
        .header("X-Emby-Authorization", auth_header(&config))
        .send()
        .map_err(|error| error.to_string())?;

    Ok(parse_json_or_error::<ItemsResponse<T>>(response, "Resposta inválida do Jellyfin.")?.Items)
}

#[tauri::command]
fn restore_session(app: AppHandle, state: State<AppState>) -> Result<Option<JellyfinConfig>, String> {
    let loaded = load_session(&app)?;
    let mut session = state
        .session
        .lock()
        .map_err(|_| "Falha ao acessar a sessão atual.".to_string())?;
    *session = loaded.clone();
    Ok(loaded)
}

#[tauri::command]
fn authenticate_jellyfin(
    app: AppHandle,
    state: State<AppState>,
    server_url: String,
    username: String,
    password: String,
) -> Result<JellyfinConfig, String> {
    let clean_url = normalize_server_url(&server_url);
    let device_id = device_id();
    let device_name = "MyFin Desktop".to_string();
    let client = blocking_client()?;

    let response = client
        .post(format!("{clean_url}/Users/AuthenticateByName"))
        .header(
            "X-Emby-Authorization",
            auth_header_without_token(&device_name, &device_id),
        )
        .json(&json!({
            "Username": username,
            "Pw": password,
        }))
        .send()
        .map_err(|error| error.to_string())?;

    let auth = parse_json_or_error::<AuthResponse>(
        response,
        "Falha na autenticação. Verifique os dados.",
    )?;

    let config = JellyfinConfig {
        server_url: clean_url,
        username: auth.User.Name,
        access_token: auth.AccessToken,
        user_id: auth.User.Id,
        device_name,
        device_id,
    };

    save_session(&app, &config)?;

    let mut session = state
        .session
        .lock()
        .map_err(|_| "Falha ao acessar a sessão atual.".to_string())?;
    *session = Some(config.clone());

    Ok(config)
}

#[tauri::command]
fn logout_jellyfin(app: AppHandle, state: State<AppState>) -> Result<(), String> {
    clear_session(&app)?;
    let mut session = state
        .session
        .lock()
        .map_err(|_| "Falha ao acessar a sessão atual.".to_string())?;
    *session = None;
    Ok(())
}

#[tauri::command]
fn get_tracks(state: State<AppState>, limit: Option<u32>) -> Result<Vec<Track>, String> {
    let config = with_session(&state)?;
    let endpoint = format!("{}/Items", config.server_url);
    get_items(
        &state,
        &endpoint,
        &[
            ("UserId", config.user_id),
            ("IncludeItemTypes", "Audio".to_string()),
            ("Recursive", "true".to_string()),
            ("Fields", "PrimaryImageAspectRatio,UserData".to_string()),
            ("Limit", limit.unwrap_or(100).to_string()),
            ("SortBy", "Random".to_string()),
        ],
    )
}

#[tauri::command]
fn get_favorites(state: State<AppState>) -> Result<Vec<Track>, String> {
    let config = with_session(&state)?;
    let endpoint = format!("{}/Users/{}/Items", config.server_url, config.user_id);
    get_items(
        &state,
        &endpoint,
        &[
            ("IncludeItemTypes", "Audio".to_string()),
            ("Recursive", "true".to_string()),
            ("Filters", "IsFavorite".to_string()),
            ("Fields", "PrimaryImageAspectRatio,UserData".to_string()),
            ("SortBy", "SortName".to_string()),
        ],
    )
}

#[tauri::command]
fn toggle_favorite(
    state: State<AppState>,
    item_id: String,
    is_favorite: bool,
) -> Result<(), String> {
    let config = with_session(&state)?;
    let client = blocking_client()?;
    let url = format!(
        "{}/Users/{}/FavoriteItems/{}",
        config.server_url, config.user_id, item_id
    );

    let request = if is_favorite {
        client.post(url)
    } else {
        client.delete(url)
    };

    let response = request
        .header("X-Emby-Authorization", auth_header(&config))
        .send()
        .map_err(|error| error.to_string())?;

    if response.status().is_success() {
        Ok(())
    } else {
        Err(response
            .text()
            .unwrap_or_else(|_| "Falha ao atualizar favorito.".to_string()))
    }
}

#[tauri::command]
fn get_playlists(state: State<AppState>) -> Result<Vec<Playlist>, String> {
    let config = with_session(&state)?;
    let endpoint = format!("{}/Users/{}/Items", config.server_url, config.user_id);
    get_items(
        &state,
        &endpoint,
        &[
            ("IncludeItemTypes", "Playlist".to_string()),
            ("Recursive", "true".to_string()),
            ("Fields", "ItemCount".to_string()),
        ],
    )
}

#[tauri::command]
fn get_playlist_items(state: State<AppState>, playlist_id: String) -> Result<Vec<Track>, String> {
    let config = with_session(&state)?;
    let endpoint = format!("{}/Playlists/{}/Items", config.server_url, playlist_id);
    get_items(
        &state,
        &endpoint,
        &[
            ("UserId", config.user_id),
            ("Fields", "PrimaryImageAspectRatio,UserData".to_string()),
        ],
    )
}

#[tauri::command]
fn create_playlist(state: State<AppState>, name: String, item_ids: Vec<String>) -> Result<(), String> {
    let config = with_session(&state)?;
    let client = blocking_client()?;
    let mut url =
        Url::parse(&format!("{}/Playlists", config.server_url)).map_err(|error| error.to_string())?;

    {
        let mut query = url.query_pairs_mut();
        query.append_pair("Name", &name);
        query.append_pair("Ids", &item_ids.join(","));
        query.append_pair("UserId", &config.user_id);
    }

    let response = client
        .post(url)
        .header("X-Emby-Authorization", auth_header(&config))
        .send()
        .map_err(|error| error.to_string())?;

    if response.status().is_success() {
        Ok(())
    } else {
        Err(response
            .text()
            .unwrap_or_else(|_| "Falha ao criar playlist.".to_string()))
    }
}

#[tauri::command]
fn get_ai_insight(track_name: String, artist_name: String) -> Result<String, String> {
    let api_key = std::env::var("GEMINI_API_KEY")
        .map_err(|_| "GEMINI_API_KEY não configurada no ambiente do Tauri.".to_string())?;
    let client = blocking_client()?;
    let prompt = format!(
        "Conte uma curiosidade curta, fascinante e pouco conhecida sobre a música \"{}\" do artista \"{}\". Seja direto e use um tom de entusiasta de música.",
        track_name, artist_name
    );

    let response = client
        .post(format!("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={api_key}"))
        .json(&json!({
            "contents": [
                {
                    "parts": [
                        { "text": prompt }
                    ]
                }
            ]
        }))
        .send()
        .map_err(|error| error.to_string())?;

    let status = response.status();
    let body = response.text().map_err(|error| error.to_string())?;
    if !status.is_success() {
        return Err(if body.trim().is_empty() {
            "Falha ao consultar o Gemini.".to_string()
        } else {
            body
        });
    }

    let payload: Value = serde_json::from_str(&body).map_err(|error| error.to_string())?;
    payload["candidates"]
        .as_array()
        .and_then(|candidates| candidates.first())
        .and_then(|candidate| candidate["content"]["parts"].as_array())
        .and_then(|parts| parts.first())
        .and_then(|part| part["text"].as_str())
        .map(|text| text.to_string())
        .ok_or_else(|| "Resposta inesperada do Gemini.".to_string())
}

fn media_response(status: StatusCode, body: Vec<u8>, content_type: &str) -> Response<Vec<u8>> {
    Response::builder()
        .status(status)
        .header(CONTENT_TYPE, content_type)
        .body(body)
        .unwrap_or_else(|_| Response::new(Vec::new()))
}

struct MediaPayload {
    status: u16,
    content_type: String,
    body: Vec<u8>,
    extra_headers: Vec<(String, String)>,
}

fn proxy_media_request(
    app: &AppHandle,
    path: &str,
    query: &str,
    range_header: Option<&str>,
) -> Result<MediaPayload, (u16, String, String)> {
    let state = app.state::<AppState>();
    let config = match with_session(&state) {
        Ok(config) => config,
        Err(message) => return Err((401, message, "text/plain".to_string())),
    };

    let mut segments = path.trim_start_matches('/').split('/');
    let kind = segments.next().unwrap_or_default();
    let item_id = segments.next().unwrap_or_default();
    let item_id = urlencoding::decode(item_id)
        .map(|value| value.into_owned())
        .unwrap_or_else(|_| item_id.to_string());

    if item_id.is_empty() {
        return Err((400, "Missing media id".to_string(), "text/plain".to_string()));
    }

    let mut target = match kind {
        "audio" => format!("{}/Audio/{item_id}/stream?static=true", config.server_url),
        "image" => format!("{}/Items/{item_id}/Images/Primary?quality=90", config.server_url),
        _ => return Err((404, "Unsupported media type".to_string(), "text/plain".to_string())),
    };

    if !query.is_empty() {
        let params = url::form_urlencoded::parse(query.as_bytes())
            .map(|(key, value)| match (kind, key.as_ref()) {
                ("audio", "quality") => format!("audioBitrate={}000", value),
                ("image", "maxWidth") => format!("maxWidth={value}"),
                _ => String::new(),
            })
            .filter(|entry| !entry.is_empty())
            .collect::<Vec<_>>();

        if !params.is_empty() {
            target.push('&');
            target.push_str(&params.join("&"));
        }
    }

    let client = match blocking_client() {
        Ok(client) => client,
        Err(message) => return Err((500, message, "text/plain".to_string())),
    };

    let mut upstream = client
        .get(target)
        .header("X-Emby-Authorization", auth_header(&config));
    if let Some(range) = range_header {
        upstream = upstream.header(RANGE, range);
    }

    let response = match upstream.send() {
        Ok(response) => response,
        Err(error) => return Err((502, error.to_string(), "text/plain".to_string())),
    };

    let status = response.status();
    let headers = response.headers().clone();
    let content_type = headers
        .get(CONTENT_TYPE)
        .and_then(|value| value.to_str().ok())
        .unwrap_or(if kind == "audio" {
            "audio/mpeg"
        } else {
            "image/jpeg"
        })
        .to_string();
    let body = match response.bytes() {
        Ok(bytes) => bytes.to_vec(),
        Err(error) => return Err((502, error.to_string(), "text/plain".to_string())),
    };

    let mut extra_headers = Vec::new();
    for header_name in [
        "accept-ranges",
        "content-range",
        "content-length",
        "cache-control",
        "etag",
        "last-modified",
    ] {
        if let Some(value) = headers.get(header_name) {
            if let Ok(as_str) = value.to_str() {
                extra_headers.push((header_name.to_string(), as_str.to_string()));
            }
        }
    }

    Ok(MediaPayload {
        status: status.as_u16(),
        content_type,
        body,
        extra_headers,
    })
}

fn proxy_media_protocol(app: &AppHandle, request: &Request<Vec<u8>>) -> Response<Vec<u8>> {
    let path = request.uri().path();
    let query = request.uri().query().unwrap_or_default();
    let range_header = request.headers().get(RANGE).and_then(|value| value.to_str().ok());

    match proxy_media_request(app, path, query, range_header) {
        Ok(payload) => {
            let mut builder = Response::builder().status(payload.status);
            builder = builder.header(CONTENT_TYPE, payload.content_type);
            for (name, value) in payload.extra_headers {
                builder = builder.header(name, value);
            }
            builder
                .body(payload.body)
                .unwrap_or_else(|_| Response::new(Vec::new()))
        }
        Err((status, message, content_type)) => {
            let code = StatusCode::from_u16(status).unwrap_or(StatusCode::INTERNAL_SERVER_ERROR);
            media_response(code, message.into_bytes(), &content_type)
        }
    }
}

fn start_media_server(app: AppHandle) -> Result<(), String> {
    let server = TinyServer::http(("127.0.0.1", MEDIA_SERVER_PORT)).map_err(|error| error.to_string())?;

    thread::spawn(move || {
        for request in server.incoming_requests() {
            if request.method() != &TinyMethod::Get {
                let response = TinyResponse::from_string("Method not allowed").with_status_code(TinyStatusCode(405));
                let _ = request.respond(response);
                continue;
            }

            let url = request.url().to_string();
            let (path, query) = match url.split_once('?') {
                Some((path, query)) => (path.to_string(), query.to_string()),
                None => (url, String::new()),
            };
            let range_header = request
                .headers()
                .iter()
                .find(|header| header.field.equiv("Range"))
                .map(|header| header.value.as_str().to_string());

            let response = match proxy_media_request(&app, &path, &query, range_header.as_deref()) {
                Ok(payload) => {
                    let mut headers = vec![
                        TinyHeader::from_bytes("Content-Type", payload.content_type).unwrap(),
                        TinyHeader::from_bytes("Access-Control-Allow-Origin", "*").unwrap(),
                        TinyHeader::from_bytes("Access-Control-Allow-Headers", "Range").unwrap(),
                    ];

                    for (name, value) in payload.extra_headers {
                        if let Ok(header) = TinyHeader::from_bytes(name, value) {
                            headers.push(header);
                        }
                    }

                    TinyResponse::new(
                        TinyStatusCode(payload.status),
                        headers,
                        Cursor::new(payload.body),
                        None,
                        None,
                    )
                }
                Err((status, message, content_type)) => TinyResponse::new(
                    TinyStatusCode(status),
                    vec![
                        TinyHeader::from_bytes("Content-Type", content_type).unwrap(),
                        TinyHeader::from_bytes("Access-Control-Allow-Origin", "*").unwrap(),
                    ],
                    Cursor::new(message.into_bytes()),
                    None,
                    None,
                ),
            };

            let _ = request.respond(response);
        }
    });

    Ok(())
}

fn main() {
    tauri::Builder::default()
        .manage(AppState::default())
        .setup(|app| {
            if let Ok(session) = load_session(&app.handle()) {
                let app_state = app.state::<AppState>();
                let mut state = app_state
                    .session
                    .lock()
                    .map_err(|_| "failed to initialize session state")?;
                *state = session;
            }
            start_media_server(app.handle().clone())?;
            Ok(())
        })
        .register_uri_scheme_protocol("media", |context, request| {
            proxy_media_protocol(context.app_handle(), &request)
        })
        .invoke_handler(tauri::generate_handler![
            restore_session,
            authenticate_jellyfin,
            logout_jellyfin,
            get_tracks,
            get_favorites,
            toggle_favorite,
            get_playlists,
            get_playlist_items,
            create_playlist,
            get_ai_insight,
        ])
        .run(tauri::generate_context!())
        .expect("error while running MyFin");
}
