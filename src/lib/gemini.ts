import { GoogleGenAI } from "@google/genai";
import { getProxyBaseUrl, isTauri } from "./runtime";

const ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY || "" });

export const getGeminiResponse = async (prompt: string) => {
  try {
    if (isTauri()) {
      const response = await fetch(`${getProxyBaseUrl()}/api/ai-insight`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ prompt }),
      });

      if (!response.ok) {
        throw new Error(await response.text());
      }

      const data = await response.json();
      return data.text as string;
    }

    const response = await ai.models.generateContent({
      model: "gemini-3-flash-preview",
      contents: prompt,
    });
    return response.text;
  } catch (error) {
    console.error("Gemini API Error:", error);
    return "Error generating response.";
  }
};
