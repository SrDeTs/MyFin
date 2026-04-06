import express from "express";
import { createServer as createViteServer } from "vite";
import path from "path";
import { createProxyMiddleware } from "http-proxy-middleware";
import cors from "cors";

async function startServer() {
  const app = express();
  const PORT = 3000;

  app.use(cors());

  // Proxy for Jellyfin to bypass CORS
  // We place this BEFORE express.json() so http-proxy-middleware can handle the body stream
  app.use('/proxy/jellyfin', (req, res, next) => {
    const urlParam = req.query.url;
    if (!urlParam || typeof urlParam !== 'string') {
      return res.status(400).send('Missing target URL');
    }

    try {
      const targetUrl = new URL(urlParam);
      // Forward other query params from the proxy request to the target
      for (const [key, value] of Object.entries(req.query)) {
        if (key !== 'url' && value !== undefined) {
          targetUrl.searchParams.append(key, String(value));
        }
      }

      createProxyMiddleware({
        target: targetUrl.origin,
        changeOrigin: true,
        ignorePath: true,
        pathRewrite: () => targetUrl.pathname + targetUrl.search,
        timeout: 15000,
        proxyTimeout: 15000,
        on: {
          error: (err, req, res) => {
            console.error('Proxy Error:', err.message);
            if ('writeHead' in res) {
              res.writeHead(502);
              res.end(`Proxy Error: ${err.message}`);
            }
          },
          proxyReq: (proxyReq, req, res) => {
            if (req.headers['x-emby-authorization']) {
              proxyReq.setHeader('X-Emby-Authorization', req.headers['x-emby-authorization']);
            }
          }
        },
        logger: console
      })(req, res, next);
    } catch (e) {
      res.status(400).send('Invalid target URL');
    }
  });

  app.use(express.json());

  // Vite middleware for development
  if (process.env.NODE_ENV !== "production") {
    const vite = await createViteServer({
      server: { middlewareMode: true },
      appType: "spa",
    });
    app.use(vite.middlewares);
  } else {
    const distPath = path.join(process.cwd(), 'dist');
    app.use(express.static(distPath));
    app.get('*', (req, res) => {
      res.sendFile(path.join(distPath, 'index.html'));
    });
  }

  app.listen(PORT, "0.0.0.0", () => {
    console.log(`Server running on http://localhost:${PORT}`);
  });
}

startServer();
