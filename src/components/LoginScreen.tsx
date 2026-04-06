import React, { useState } from 'react';
import { motion } from 'motion/react';
import { Music, Server, User, Lock, ChevronRight } from 'lucide-react';
import { jellyfin, JellyfinConfig } from '../services/jellyfin';
import { cn } from '../lib/utils';
import { isTauri } from '../lib/runtime';
import { SynesthesiaBackground } from './SynesthesiaBackground';
import { ThemeSwitch } from './ui/ThemeSwitch';

interface LoginScreenProps {
  onLogin: (config: JellyfinConfig) => void;
  isDark: boolean;
  onToggleTheme: () => void;
  bgAnimationEnabled: boolean;
  bgAnimationSpeed: string;
}

export const LoginScreen: React.FC<LoginScreenProps> = ({ 
  onLogin, 
  isDark, 
  onToggleTheme, 
  bgAnimationEnabled, 
  bgAnimationSpeed 
}) => {
  const [serverUrl, setServerUrl] = useState('');
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault();
    
    if (!isTauri() && (serverUrl.includes('192.168.') || serverUrl.includes('127.0.0.1') || serverUrl.includes('localhost'))) {
      setError('IPs locais não funcionam na nuvem. Use uma URL pública (ex: Cloudflare Tunnel ou Ngrok).');
      return;
    }

    setLoading(true);
    setError('');
    try {
      const config = await jellyfin.authenticate(serverUrl, username, password);
      onLogin(config);
    } catch (err: any) {
      console.error('Login error:', err);
      const msg = err.message || '';
      
      if (msg === 'WARMUP_REQUIRED') {
        setError('O servidor Jellyfin está "dormindo". Clique no botão abaixo para acordá-lo e tente novamente em alguns segundos.');
      } else if (msg === 'HTML_RESPONSE') {
        setError('O servidor retornou uma página em vez de dados. Verifique se a URL está correta.');
      } else if (err.response?.data && typeof err.response.data === 'string') {
        setError(err.response.data);
      } else {
        setError(err.message || 'Erro desconhecido ao conectar.');
      }
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="fixed inset-0 flex items-center justify-center bg-transparent z-50">
      <SynesthesiaBackground data={new Array(40).fill(0)} accentColor="purple" />
      <div className="fixed inset-0 -z-20 bg-[#050505]" />
      
      {bgAnimationEnabled && (
        <div className={cn(
          "app-background-animation",
          bgAnimationSpeed === 'slow' && "bg-speed-slow",
          bgAnimationSpeed === 'fast' && "bg-speed-fast"
        )}>
          <div className="bg-ring-container">
            <div className="bg-ring"></div>
            <div className="bg-ring"></div>
            <div className="bg-ring"></div>
          </div>
        </div>
      )}
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] bg-purple-900/20 blur-[120px] rounded-full" />
        <div className="absolute bottom-[-10%] right-[-10%] w-[40%] h-[40%] bg-blue-900/20 blur-[120px] rounded-full" />
      </div>

      <div className="absolute top-8 right-8 z-20">
        <ThemeSwitch isDark={isDark} onToggle={onToggleTheme} />
      </div>

      <motion.div 
        initial={{ opacity: 0, y: 20 }}
        animate={{ opacity: 1, y: 0 }}
        className="w-full max-w-md p-8 glass rounded-[40px] relative z-10"
      >
        <div className="text-center mb-8">
          <div className="w-16 h-16 bg-gradient-to-tr from-purple-600 to-blue-500 rounded-2xl flex items-center justify-center mx-auto mb-4 shadow-lg shadow-purple-500/20">
            <Music className="text-white w-8 h-8" />
          </div>
          <h1 className="text-3xl font-display font-bold tracking-tight">MyFin</h1>
          <p className="text-white/50 text-sm mt-2 dark:text-white/50 light:text-black/50">Conecte-se ao seu servidor Jellyfin</p>
        </div>

        <form onSubmit={handleLogin} className="space-y-4">
          <div className="space-y-2">
            <label className="text-xs font-medium text-white/40 uppercase tracking-wider ml-1">Servidor</label>
            <div className="relative">
              <Server className="absolute left-4 top-1/2 -translate-y-1/2 w-4 h-4 text-white/30" />
              <input 
                type="url" 
                placeholder="https://jellyfin.exemplo.com"
                className="w-full bg-white/5 border border-white/10 rounded-xl py-3 pl-12 pr-4 focus:outline-none focus:ring-2 focus:ring-purple-500/50 transition-all"
                value={serverUrl}
                onChange={(e) => setServerUrl(e.target.value)}
                required
              />
            </div>
          </div>

          <div className="space-y-2">
            <label className="text-xs font-medium text-white/40 uppercase tracking-wider ml-1">Usuário</label>
            <div className="relative">
              <User className="absolute left-4 top-1/2 -translate-y-1/2 w-4 h-4 text-white/30" />
              <input 
                type="text" 
                placeholder="Seu usuário"
                className="w-full bg-white/5 border border-white/10 rounded-xl py-3 pl-12 pr-4 focus:outline-none focus:ring-2 focus:ring-purple-500/50 transition-all"
                value={username}
                onChange={(e) => setUsername(e.target.value)}
                required
              />
            </div>
          </div>

          <div className="space-y-2">
            <label className="text-xs font-medium text-white/40 uppercase tracking-wider ml-1">Senha</label>
            <div className="relative">
              <Lock className="absolute left-4 top-1/2 -translate-y-1/2 w-4 h-4 text-white/30" />
              <input 
                type="password" 
                placeholder="••••••••"
                className="w-full bg-white/5 border border-white/10 rounded-xl py-3 pl-12 pr-4 focus:outline-none focus:ring-2 focus:ring-purple-500/50 transition-all"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                required
              />
            </div>
          </div>

          {error && (
            <div className="p-4 rounded-xl bg-red-500/10 border border-red-500/20">
              <p className="text-red-400 text-xs text-center leading-relaxed">{error}</p>
              {error.includes('acordá-lo') && (
                <a 
                  href={serverUrl} 
                  target="_blank" 
                  rel="noopener noreferrer"
                  className="block w-full text-center mt-2 py-2 bg-red-500/20 hover:bg-red-500/30 text-red-400 text-xs font-bold rounded-lg transition-colors"
                >
                  ACORDAR SERVIDOR ↗
                </a>
              )}
            </div>
          )}

          <button 
            type="submit" 
            disabled={loading}
            className="w-full bg-white text-black font-bold py-3 rounded-xl mt-4 hover:bg-white/90 transition-all active:scale-[0.98] disabled:opacity-50 flex items-center justify-center gap-2"
          >
            {loading ? 'Conectando...' : 'Entrar'}
            {!loading && <ChevronRight className="w-4 h-4" />}
          </button>
        </form>
      </motion.div>
    </div>
  );
};
