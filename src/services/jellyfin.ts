import { invoke } from '@tauri-apps/api/core';

const MEDIA_SERVER_ORIGIN = 'http://127.0.0.1:29345';

export interface JellyfinConfig {
  serverUrl: string;
  username: string;
  accessToken: string;
  userId: string;
  deviceName: string;
  deviceId: string;
}

export interface Track {
  Id: string;
  Name: string;
  ArtistItems: { Name: string }[];
  Album?: string;
  RunTimeTicks?: number;
  ImageTags?: { Primary?: string };
  UserData?: {
    IsFavorite: boolean;
    PlaybackPositionTicks?: number;
    PlayCount?: number;
  };
}

export interface Playlist {
  Id: string;
  Name: string;
  ItemCount?: number;
}

export class JellyfinService {
  private config: JellyfinConfig | null = null;

  async authenticate(serverUrl: string, username: string, password: string): Promise<JellyfinConfig> {
    const config = await invoke<JellyfinConfig>('authenticate_jellyfin', {
      serverUrl,
      username,
      password,
    });
    this.config = config;
    return config;
  }

  async restoreSession(): Promise<JellyfinConfig | null> {
    const config = await invoke<JellyfinConfig | null>('restore_session');
    this.config = config;
    return config;
  }

  async logout(): Promise<void> {
    await invoke('logout_jellyfin');
    this.config = null;
  }

  isLoggedIn() {
    return !!this.config;
  }

  getConfig(): JellyfinConfig | null {
    return this.config;
  }

  async getTracks(limit = 50): Promise<Track[]> {
    return invoke<Track[]>('get_tracks', { limit });
  }

  getStreamUrl(trackId: string, bitrate?: number): string {
    if (!this.config) return '';

    const params = new URLSearchParams();
    if (bitrate) {
      params.set('quality', String(bitrate));
    }

    const suffix = params.toString();
    return `${MEDIA_SERVER_ORIGIN}/audio/${encodeURIComponent(trackId)}${suffix ? `?${suffix}` : ''}`;
  }

  getAlbumArtUrl(trackId: string, maxWidth = 400): string {
    if (!this.config) return '';
    return `${MEDIA_SERVER_ORIGIN}/image/${encodeURIComponent(trackId)}?maxWidth=${maxWidth}`;
  }

  async getFavorites(): Promise<Track[]> {
    return invoke<Track[]>('get_favorites');
  }

  async toggleFavorite(itemId: string, isFavorite: boolean): Promise<void> {
    await invoke('toggle_favorite', { itemId, isFavorite });
  }

  async getPlaylists(): Promise<Playlist[]> {
    return invoke<Playlist[]>('get_playlists');
  }

  async getPlaylistItems(playlistId: string): Promise<Track[]> {
    return invoke<Track[]>('get_playlist_items', { playlistId });
  }

  async createPlaylist(name: string, itemIds?: string[]): Promise<void> {
    await invoke('create_playlist', {
      name,
      itemIds: itemIds || [],
    });
  }
}

export const jellyfin = new JellyfinService();
