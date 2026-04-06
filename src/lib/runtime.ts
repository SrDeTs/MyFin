declare global {
  interface Window {
    __TAURI_INTERNALS__?: unknown;
  }
}

export const TAURI_LOCAL_SERVER = 'http://127.0.0.1:31457';

export function isTauri(): boolean {
  return typeof window !== 'undefined' && '__TAURI_INTERNALS__' in window;
}

export function getProxyBaseUrl(): string {
  return isTauri() ? TAURI_LOCAL_SERVER : window.location.origin;
}
