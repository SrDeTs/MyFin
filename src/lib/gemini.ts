import { invoke } from '@tauri-apps/api/core';

export const getGeminiResponse = async (trackName: string, artistName: string) => {
  return invoke<string>('get_ai_insight', {
    trackName,
    artistName,
  });
};
