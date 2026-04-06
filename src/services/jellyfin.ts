import axios from 'axios';

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
  Album: string;
  RunTimeTicks: number;
  ImageTags: { Primary?: string };
  UserData?: {
    IsFavorite: boolean;
    PlaybackPositionTicks: number;
    PlayCount: number;
  };
}

export interface Playlist {
  Id: string;
  Name: string;
  ItemCount: number;
}

export class JellyfinService {
  private config: JellyfinConfig | null = null;

  constructor() {
    const saved = localStorage.getItem('aura_jellyfin_config');
    if (saved) {
      this.config = JSON.parse(saved);
    }
  }

  private get authHeader() {
    if (!this.config) return '';
    return `MediaBrowser Client="Aura Music", Device="${this.config.deviceName}", DeviceId="${this.config.deviceId}", Version="1.0.0", Token="${this.config.accessToken}"`;
  }

  private getProxyUrl(url: string): string {
    if (!url) return '';
    const baseUrl = window.location.origin;
    return `${baseUrl}/proxy/jellyfin?url=${encodeURIComponent(url)}`;
  }

  private async request(method: 'get' | 'post' | 'delete', url: string, data?: any, headers?: any) {
    // Try direct connection first (works if CORS is enabled on server)
    try {
      return await axios({ 
        method, 
        url, 
        data, 
        headers, 
        timeout: 5000 
      });
    } catch (err: any) {
      // If it's a network error or CORS issue, try proxy
      const isNetworkError = !err.response || err.code === 'ERR_NETWORK' || err.message === 'Network Error';
      
      if (isNetworkError) {
        console.log('Direct connection failed, trying proxy...', url);
        const proxyUrl = this.getProxyUrl(url);
        return await axios({ 
          method, 
          url: proxyUrl, 
          data, 
          headers, 
          timeout: 15000 
        });
      }
      throw err;
    }
  }

  async authenticate(serverUrl: string, username: string, password: string): Promise<JellyfinConfig> {
    const cleanUrl = serverUrl.replace(/\/$/, '');
    const deviceId = 'aura-web-player-' + Math.random().toString(36).substring(7);
    const deviceName = 'Aura Web Player';

    const authHeader = `MediaBrowser Client="Aura Music", Device="${deviceName}", DeviceId="${deviceId}", Version="1.0.0"`;

    try {
      const authUrl = `${cleanUrl}/Users/AuthenticateByName`;
      const response = await this.request('post', authUrl, {
        Username: username,
        Pw: password
      }, {
        'X-Emby-Authorization': authHeader
      });

      if (!response.data || !response.data.User) {
        console.error('Unexpected Jellyfin response:', response.data);
        
        const responseStr = typeof response.data === 'string' ? response.data : JSON.stringify(response.data);
        
        if (responseStr.includes('<title>Starting Server...</title>') || responseStr.includes('Please wait while your application starts')) {
          throw new Error('WARMUP_REQUIRED');
        }
        if (responseStr.startsWith('<!doctype html>') || responseStr.includes('<html')) {
          throw new Error('HTML_RESPONSE');
        }
        
        if (responseStr.includes('Proxy Error')) {
          throw new Error(responseStr);
        }
        throw new Error('Resposta inválida do servidor Jellyfin. Verifique a URL.');
      }

      const config: JellyfinConfig = {
        serverUrl: cleanUrl,
        username: response.data.User.Name,
        accessToken: response.data.AccessToken,
        userId: response.data.User.Id,
        deviceName,
        deviceId
      };

      this.config = config;
      localStorage.setItem('aura_jellyfin_config', JSON.stringify(config));
      return config;
    } catch (error: any) {
      console.error('Jellyfin Auth Error:', error);
      if (error.response?.data && typeof error.response.data === 'string') {
        throw new Error(error.response.data);
      }
      if (error.message) throw error;
      throw new Error('Falha na autenticação. Verifique os dados.');
    }
  }

  logout() {
    this.config = null;
    localStorage.removeItem('aura_jellyfin_config');
  }

  isLoggedIn() {
    return !!this.config;
  }

  getConfig(): JellyfinConfig | null {
    return this.config;
  }

  async getTracks(limit = 50): Promise<Track[]> {
    if (!this.config) throw new Error('Not authenticated');

    const tracksUrl = `${this.config.serverUrl}/Items`;
    const urlWithParams = new URL(tracksUrl);
    urlWithParams.searchParams.append('UserId', this.config.userId);
    urlWithParams.searchParams.append('IncludeItemTypes', 'Audio');
    urlWithParams.searchParams.append('Recursive', 'true');
    urlWithParams.searchParams.append('Fields', 'PrimaryImageAspectRatio,CanDelete,BasicSyncInfo');
    urlWithParams.searchParams.append('Limit', limit.toString());
    urlWithParams.searchParams.append('SortBy', 'Random');

    const response = await this.request('get', urlWithParams.toString(), undefined, {
      'X-Emby-Authorization': this.authHeader
    });

    return response.data.Items || [];
  }

  getStreamUrl(trackId: string, bitrate?: number): string {
    if (!this.config) return '';
    if (!bitrate) {
      return `${this.config.serverUrl}/Audio/${trackId}/stream?static=true&api_key=${this.config.accessToken}`;
    }
    // For transcoding, we need more params usually, but bitrate is the key one here
    return `${this.config.serverUrl}/Audio/${trackId}/stream?audioBitrate=${bitrate * 1000}&api_key=${this.config.accessToken}`;
  }

  getAlbumArtUrl(trackId: string, maxWidth = 400): string {
    if (!this.config) return '';
    return `${this.config.serverUrl}/Items/${trackId}/Images/Primary?maxWidth=${maxWidth}&quality=90`;
  }

  async getFavorites(): Promise<Track[]> {
    if (!this.config) throw new Error('Not authenticated');

    const url = `${this.config.serverUrl}/Users/${this.config.userId}/Items`;
    const params = new URLSearchParams({
      IncludeItemTypes: 'Audio',
      Recursive: 'true',
      Filters: 'IsFavorite',
      Fields: 'PrimaryImageAspectRatio,UserData',
      SortBy: 'SortName'
    });

    const response = await this.request('get', `${url}?${params.toString()}`, undefined, {
      'X-Emby-Authorization': this.authHeader
    });

    return response.data.Items || [];
  }

  async toggleFavorite(itemId: string, isFavorite: boolean): Promise<void> {
    if (!this.config) throw new Error('Not authenticated');

    const url = `${this.config.serverUrl}/Users/${this.config.userId}/FavoriteItems/${itemId}`;
    const method = isFavorite ? 'post' : 'delete';

    await this.request(method, url, undefined, {
      'X-Emby-Authorization': this.authHeader
    });
  }

  async getPlaylists(): Promise<Playlist[]> {
    if (!this.config) throw new Error('Not authenticated');

    const url = `${this.config.serverUrl}/Users/${this.config.userId}/Items`;
    const params = new URLSearchParams({
      IncludeItemTypes: 'Playlist',
      Recursive: 'true',
      Fields: 'ItemCount'
    });

    const response = await this.request('get', `${url}?${params.toString()}`, undefined, {
      'X-Emby-Authorization': this.authHeader
    });

    return response.data.Items || [];
  }

  async getPlaylistItems(playlistId: string): Promise<Track[]> {
    if (!this.config) throw new Error('Not authenticated');

    const url = `${this.config.serverUrl}/Playlists/${playlistId}/Items`;
    const params = new URLSearchParams({
      UserId: this.config.userId,
      Fields: 'PrimaryImageAspectRatio,UserData'
    });

    const response = await this.request('get', `${url}?${params.toString()}`, undefined, {
      'X-Emby-Authorization': this.authHeader
    });

    return response.data.Items || [];
  }

  async createPlaylist(name: string, itemIds?: string[]): Promise<void> {
    if (!this.config) throw new Error('Not authenticated');

    const url = `${this.config.serverUrl}/Playlists`;
    const params = new URLSearchParams({
      Name: name,
      Ids: (itemIds || []).join(','),
      UserId: this.config.userId
    });

    await this.request('post', `${url}?${params.toString()}`, undefined, {
      'X-Emby-Authorization': this.authHeader
    });
  }
}

export const jellyfin = new JellyfinService();
