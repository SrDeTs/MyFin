use std::{net::SocketAddr, sync::Arc};

use axum::{
  body::{to_bytes, Body},
  extract::{Query, State},
  http::{HeaderMap, HeaderName, HeaderValue, Method, StatusCode},
  response::IntoResponse,
  routing::{any, post},
  Json, Router,
};
use reqwest::Client;
use serde::Deserialize;
use serde_json::{json, Value};

const LOCAL_SERVER_ADDR: &str = "127.0.0.1:31457";

#[derive(Clone)]
struct AppState {
  http: Client,
  gemini_api_key: Option<String>,
}

#[derive(Deserialize)]
struct ProxyQuery {
  url: String,
}

#[derive(Deserialize)]
struct AiInsightRequest {
  prompt: String,
}

fn cors_headers() -> HeaderMap {
  let mut headers = HeaderMap::new();
  headers.insert(
    HeaderName::from_static("access-control-allow-origin"),
    HeaderValue::from_static("*"),
  );
  headers.insert(
    HeaderName::from_static("access-control-allow-headers"),
    HeaderValue::from_static("*"),
  );
  headers.insert(
    HeaderName::from_static("access-control-allow-methods"),
    HeaderValue::from_static("*"),
  );
  headers
}

async fn proxy_jellyfin(
  State(state): State<Arc<AppState>>,
  Query(query): Query<ProxyQuery>,
  method: Method,
  headers: HeaderMap,
  body: Body,
) -> Result<impl IntoResponse, (StatusCode, String)> {
  let target = reqwest::Url::parse(&query.url)
    .map_err(|err| (StatusCode::BAD_REQUEST, format!("Invalid target URL: {err}")))?;

  let mut request = state.http.request(method, target);

  if let Some(auth) = headers.get("x-emby-authorization") {
    request = request.header("X-Emby-Authorization", auth);
  }

  if let Some(content_type) = headers.get("content-type") {
    request = request.header("Content-Type", content_type);
  }

  let bytes = to_bytes(body, usize::MAX)
    .await
    .map_err(|err| (StatusCode::BAD_REQUEST, format!("Failed to read body: {err}")))?;

  if !bytes.is_empty() {
    request = request.body(bytes);
  }

  let upstream = request
    .send()
    .await
    .map_err(|err| (StatusCode::BAD_GATEWAY, format!("Proxy request failed: {err}")))?;

  let status = upstream.status();
  let upstream_headers = upstream.headers().clone();
  let body = upstream
    .bytes()
    .await
    .map_err(|err| (StatusCode::BAD_GATEWAY, format!("Failed to read upstream body: {err}")))?;

  let mut response_headers = cors_headers();
  if let Some(content_type) = upstream_headers.get("content-type") {
    response_headers.insert("content-type", content_type.clone());
  }
  if let Some(content_length) = upstream_headers.get("content-length") {
    response_headers.insert("content-length", content_length.clone());
  }

  Ok((status, response_headers, body))
}

async fn options_handler() -> impl IntoResponse {
  (StatusCode::NO_CONTENT, cors_headers())
}

async fn ai_insight(
  State(state): State<Arc<AppState>>,
  Json(payload): Json<AiInsightRequest>,
) -> Result<impl IntoResponse, (StatusCode, String)> {
  let Some(api_key) = state.gemini_api_key.as_deref() else {
    return Err((
      StatusCode::INTERNAL_SERVER_ERROR,
      "GEMINI_API_KEY is not configured".to_string(),
    ));
  };

  let response = state
    .http
    .post("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent")
    .query(&[("key", api_key)])
    .json(&json!({
      "contents": [
        {
          "parts": [
            {
              "text": payload.prompt
            }
          ]
        }
      ]
    }))
    .send()
    .await
    .map_err(|err| (StatusCode::BAD_GATEWAY, format!("Gemini request failed: {err}")))?;

  let status = response.status();
  let data: Value = response
    .json()
    .await
    .map_err(|err| (StatusCode::BAD_GATEWAY, format!("Invalid Gemini response: {err}")))?;

  if !status.is_success() {
    return Err((status, data.to_string()));
  }

  let text = data["candidates"][0]["content"]["parts"][0]["text"]
    .as_str()
    .unwrap_or("Não consegui encontrar curiosidades sobre esta faixa.")
    .to_string();

  Ok((cors_headers(), Json(json!({ "text": text }))))
}

fn spawn_local_server() {
  tauri::async_runtime::spawn(async move {
    let state = Arc::new(AppState {
      http: Client::builder()
        .build()
        .expect("failed to build HTTP client"),
      gemini_api_key: std::env::var("GEMINI_API_KEY").ok(),
    });

    let app = Router::new()
      .route("/proxy/jellyfin", any(proxy_jellyfin).options(options_handler))
      .route("/api/ai-insight", post(ai_insight).options(options_handler))
      .with_state(state);

    let addr: SocketAddr = LOCAL_SERVER_ADDR.parse().expect("invalid local server addr");
    let listener = tokio::net::TcpListener::bind(addr)
      .await
      .expect("failed to bind local proxy server");

    axum::serve(listener, app)
      .await
      .expect("local proxy server crashed");
  });
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
  tauri::Builder::default()
    .setup(|app| {
      spawn_local_server();

      if cfg!(debug_assertions) {
        app.handle().plugin(
          tauri_plugin_log::Builder::default()
            .level(log::LevelFilter::Info)
            .build(),
        )?;
      }
      Ok(())
    })
    .run(tauri::generate_context!())
    .expect("error while running tauri application");
}
