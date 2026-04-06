import React, { useState, useEffect, useRef } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { 
  Play, Pause, SkipBack, SkipForward, Volume2, Search, 
  Music, LogOut, Settings, Heart, ListMusic, Disc,
  Server, User, Lock, ChevronRight, LayoutGrid,
  Shuffle, Repeat, Maximize2, X, Check, ChevronDown,
  Loader2, Save, Palette, Clock, Globe, Bell, 
  MessageSquare, Zap, FastForward, PlayCircle, Activity,
  Languages, Type, Image as ImageIcon, Sparkles
} from 'lucide-react';
import { jellyfin, Track, JellyfinConfig, Playlist } from './services/jellyfin';
import { getGeminiResponse } from './lib/gemini';
import { cn, formatTime } from './lib/utils';

// --- Components ---

const SynesthesiaBackground: React.FC<{ data: number[], accentColor: string, zIndex?: string, enabled?: boolean }> = ({ data, accentColor, zIndex = "-z-10", enabled = true }) => {
  if (!enabled) return null;
  
  const avg = data.reduce((a, b) => a + b, 0) / (data.length || 1);
  const bass = data.slice(0, 5).reduce((a, b) => a + b, 0) / 5;
  const treble = data.slice(30, 40).reduce((a, b) => a + b, 0) / 10;

  return (
    <div className={cn("fixed inset-0 overflow-hidden pointer-events-none", zIndex)}>
      <motion.div 
        animate={{ 
          scale: 1 + bass * 1.2,
          opacity: 0.3 + avg * 0.5,
          rotate: avg * 1080
        }}
        transition={{ type: "spring", bounce: 0, duration: 0.5 }}
        className={cn(
          "absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[180vw] h-[180vw] rounded-full blur-[160px]",
          accentColor === 'purple' && "bg-purple-600/60",
          accentColor === 'blue' && "bg-blue-600/60",
          accentColor === 'green' && "bg-green-600/60",
          accentColor === 'pink' && "bg-pink-600/60",
          accentColor === 'orange' && "bg-orange-600/60"
        )}
      />
      <motion.div 
        animate={{ 
          scale: 1 + treble * 1.5,
          opacity: 0.2 + treble * 0.4,
          x: treble * 300 - 150,
          y: treble * 300 - 150
        }}
        className="absolute top-1/4 left-1/4 w-[100vw] h-[100vw] bg-white/30 rounded-full blur-[140px]"
      />
    </div>
  );
};

const PlasmaVolume = ({ 
  value, 
  onChange, 
  orientation = 'horizontal',
  className 
}: { 
  value: number, 
  onChange: (v: number) => void, 
  orientation?: 'horizontal' | 'vertical',
  className?: string 
}) => {
  const containerRef = useRef<HTMLDivElement>(null);
  const [isDragging, setIsDragging] = useState(false);

  const handleInteraction = (e: React.MouseEvent | React.TouchEvent | MouseEvent | TouchEvent) => {
    if (!containerRef.current) return;
    const rect = containerRef.current.getBoundingClientRect();
    let newValue: number;

    if (orientation === 'horizontal') {
      const clientX = 'touches' in e ? (e as TouchEvent).touches[0].clientX : (e as MouseEvent).clientX;
      const x = Math.max(0, Math.min(clientX - rect.left, rect.width));
      newValue = x / rect.width;
    } else {
      const clientY = 'touches' in e ? (e as TouchEvent).touches[0].clientY : (e as MouseEvent).clientY;
      const y = Math.max(0, Math.min(clientY - rect.top, rect.height));
      newValue = 1 - (y / rect.height);
    }
    onChange(newValue);
  };

  useEffect(() => {
    const handleMove = (e: MouseEvent | TouchEvent) => {
      if (isDragging) {
        handleInteraction(e);
      }
    };
    const handleUp = () => setIsDragging(false);

    if (isDragging) {
      window.addEventListener('mousemove', handleMove);
      window.addEventListener('mouseup', handleUp);
      window.addEventListener('touchmove', handleMove);
      window.addEventListener('touchend', handleUp);
    }
    return () => {
      window.removeEventListener('mousemove', handleMove);
      window.removeEventListener('mouseup', handleUp);
      window.removeEventListener('touchmove', handleMove);
      window.removeEventListener('touchend', handleUp);
    };
  }, [isDragging]);

  return (
    <div 
      ref={containerRef}
      className={cn(
        "plasma-container relative cursor-pointer",
        orientation === 'horizontal' ? "w-32 h-6" : "w-10 h-48",
        className
      )}
      onMouseDown={(e) => {
        setIsDragging(true);
        handleInteraction(e);
      }}
      onTouchStart={(e) => {
        setIsDragging(true);
        handleInteraction(e);
      }}
    >
      <div className="plasma-track">
        {/* Ticks */}
        <div className={cn(
          "plasma-ticks absolute inset-0 px-2",
          orientation === 'horizontal' ? "flex-row py-1" : "flex-col py-2"
        )}>
          {[...Array(8)].map((_, i) => (
            <div 
              key={i} 
              className={cn(
                "plasma-tick",
                orientation === 'horizontal' ? "w-[1px] h-full" : "h-[1px] w-full",
                (i / 7) <= value && "active"
              )} 
            />
          ))}
        </div>

        {/* Fill */}
        <div className="plasma-fill-container">
          <div 
            className="plasma-fill absolute"
            style={{
              ...(orientation === 'horizontal' 
                ? { left: 0, bottom: 0, top: 0, width: `${value * 100}%` }
                : { left: 0, right: 0, bottom: 0, height: `${value * 100}%` }
              )
            }}
          />
        </div>

        {/* Knob */}
        <div 
          className="plasma-knob"
          style={{
            width: orientation === 'horizontal' ? '20px' : '28px',
            height: orientation === 'horizontal' ? '20px' : '28px',
            ...(orientation === 'horizontal'
              ? { left: `${value * 100}%`, top: '50%', transform: 'translate(-50%, -50%)' }
              : { left: '50%', bottom: `${value * 100}%`, transform: 'translate(-50%, 50%)' }
            )
          }}
        />
      </div>
    </div>
  );
};

const ThemeSwitch = ({ isDark, onToggle }: { isDark: boolean, onToggle: () => void }) => {
  return (
    <label className="switch theme-switch-container">
      <input 
        className="switch__input" 
        type="checkbox" 
        role="switch" 
        checked={isDark}
        onChange={onToggle}
      />
      <span className="switch__icon">
        <span className="switch__icon-part switch__icon-part--1"></span>
        <span className="switch__icon-part switch__icon-part--2"></span>
        <span className="switch__icon-part switch__icon-part--3"></span>
        <span className="switch__icon-part switch__icon-part--4"></span>
        <span className="switch__icon-part switch__icon-part--5"></span>
        <span className="switch__icon-part switch__icon-part--6"></span>
        <span className="switch__icon-part switch__icon-part--7"></span>
        <span className="switch__icon-part switch__icon-part--8"></span>
        <span className="switch__icon-part switch__icon-part--9"></span>
        <span className="switch__icon-part switch__icon-part--10"></span>
        <span className="switch__icon-part switch__icon-part--11"></span>
      </span>
      <span className="switch__sr">Dark Mode</span>
    </label>
  );
};

const LoginScreen = ({ onLogin, isDark, onToggleTheme, bgAnimationEnabled, bgAnimationSpeed }: { onLogin: (config: JellyfinConfig) => void, isDark: boolean, onToggleTheme: () => void, bgAnimationEnabled: boolean, bgAnimationSpeed: string }) => {
  const [serverUrl, setServerUrl] = useState('');
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault();

    setLoading(true);
    setError('');
    try {
      const config = await jellyfin.authenticate(serverUrl, username, password);
      onLogin(config);
    } catch (err: any) {
      console.error('Login error:', err);
      const msg = typeof err === 'string'
        ? err
        : err?.message || err?.toString?.() || '';
      
      if (msg === 'WARMUP_REQUIRED') {
        setError('O servidor Jellyfin está "dormindo". Clique no botão abaixo para acordá-lo e tente novamente em alguns segundos.');
      } else if (msg === 'HTML_RESPONSE') {
        setError('O servidor retornou uma página em vez de dados. Verifique se a URL está correta.');
      } else {
        setError(msg || 'Erro desconhecido ao conectar.');
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

export default function App() {
  const webAudioEnabled = false;
  const [isLoggedIn, setIsLoggedIn] = useState(false);
  const [sessionConfig, setSessionConfig] = useState<JellyfinConfig | null>(null);
  const [isSessionLoading, setIsSessionLoading] = useState(true);
  const [tracks, setTracks] = useState<Track[]>([]);
  const [currentTrack, setCurrentTrack] = useState<Track | null>(null);
  const [isPlaying, setIsPlaying] = useState(false);
  const [progress, setProgress] = useState(0);
  const [duration, setDuration] = useState(0);
  const [volume, setVolume] = useState(0.8);
  const [searchQuery, setSearchQuery] = useState('');
  const [activeView, setActiveView] = useState<'explore' | 'albums' | 'artists' | 'favorites' | 'playlists'>('explore');
  const [isSettingsOpen, setIsSettingsOpen] = useState(false);
  const [isRefreshing, setIsRefreshing] = useState(false);
  const [isNowPlayingOpen, setIsNowPlayingOpen] = useState(false);
  const [isDark, setIsDark] = useState(() => localStorage.getItem('aura_theme') !== 'light');
  const [audioQuality, setAudioQuality] = useState<'Auto' | '320kbps' | '128kbps'>(() => (localStorage.getItem('aura_audio_quality') as any) || 'Auto');
  const [immersiveMode, setImmersiveMode] = useState(() => localStorage.getItem('aura_immersive_mode') === 'true');
  const [volumeNormalization, setVolumeNormalization] = useState(() => localStorage.getItem('aura_volume_normalization') === 'true');
  const [settingsTab, setSettingsTab] = useState<'interface' | 'audio' | 'soundlab' | 'advanced' | 'account'>('interface');
  const [isSaving, setIsSaving] = useState(false);
  const [saveSuccess, setSaveSuccess] = useState(false);
  const [isShuffle, setIsShuffle] = useState(false);
  const [isRepeat, setIsRepeat] = useState(false);
  const [favoriteTracks, setFavoriteTracks] = useState<Track[]>([]);
  const [playlists, setPlaylists] = useState<Playlist[]>([]);
  const [activePlaylist, setActivePlaylist] = useState<Playlist | null>(null);
  const [visualizerData, setVisualizerData] = useState<number[]>(new Array(40).fill(0));
  const [synesthesiaEnabled, setSynesthesiaEnabled] = useState(() => localStorage.getItem('aura_synesthesia_enabled') !== 'false');
  const [audioFilter, setAudioFilter] = useState<'none' | 'vintage' | 'vinyl' | 'lofi'>('none');
  const [isAiLoading, setIsAiLoading] = useState(false);
  const [aiInsight, setAiInsight] = useState<string | null>(null);
  
  // --- New Settings State ---
  const [accentColor, setAccentColor] = useState<'purple' | 'blue' | 'green' | 'pink' | 'orange'>(() => (localStorage.getItem('aura_accent_color') as any) || 'purple');
  const [bgAnimationEnabled, setBgAnimationEnabled] = useState(() => localStorage.getItem('aura_bg_anim_enabled') !== 'false');
  const [bgAnimationSpeed, setBgAnimationSpeed] = useState<'slow' | 'normal' | 'fast'>(() => (localStorage.getItem('aura_bg_anim_speed') as any) || 'normal');
  const [albumArtStyle, setAlbumArtStyle] = useState<'square' | 'round'>(() => (localStorage.getItem('aura_album_art_style') as any) || 'square');
  const [fontFamily, setFontFamily] = useState<'sans' | 'display' | 'serif'>(() => (localStorage.getItem('aura_font_family') as any) || 'sans');
  const [equalizerPreset, setEqualizerPreset] = useState<'none' | 'bass' | 'rock' | 'pop' | 'vocal'>(() => (localStorage.getItem('aura_eq_preset') as any) || 'none');
  const [crossfadeTime, setCrossfadeTime] = useState(() => parseInt(localStorage.getItem('aura_crossfade_time') || '0'));
  const [sleepTimer, setSleepTimer] = useState<number | null>(null);
  const [autoPlay, setAutoPlay] = useState(() => localStorage.getItem('aura_auto_play') === 'true');
  const [showLyrics, setShowLyrics] = useState(false);
  const [notificationsEnabled, setNotificationsEnabled] = useState(() => localStorage.getItem('aura_notifications') === 'true');
  const [lastfmEnabled, setLastfmEnabled] = useState(() => localStorage.getItem('aura_lastfm') === 'true');
  const [language, setLanguage] = useState<'pt' | 'en' | 'es'>(() => (localStorage.getItem('aura_language') as any) || 'pt');
  
  const audioRef = useRef<HTMLAudioElement | null>(null);
  const sleepTimerRef = useRef<NodeJS.Timeout | null>(null);
  const analyserRef = useRef<AnalyserNode | null>(null);
  const dataArrayRef = useRef<Uint8Array | null>(null);
  const animationFrameRef = useRef<number | null>(null);
  const audioContextRef = useRef<AudioContext | null>(null);
  const filterNodeRef = useRef<BiquadFilterNode | null>(null);
  const filterNode2Ref = useRef<BiquadFilterNode | null>(null);

  useEffect(() => {
    let isMounted = true;

    jellyfin.restoreSession()
      .then((config) => {
        if (!isMounted) return;
        setSessionConfig(config);
        setIsLoggedIn(!!config);
      })
      .catch((error) => {
        console.error('Error restoring session:', error);
      })
      .finally(() => {
        if (isMounted) {
          setIsSessionLoading(false);
        }
      });

    return () => {
      isMounted = false;
    };
  }, []);

  useEffect(() => {
    if (!webAudioEnabled) return;
    if (!audioRef.current || !isLoggedIn) return;

    const initAudioContext = () => {
      if (audioContextRef.current) return;
      
      const audioContext = new (window.AudioContext || (window as any).webkitAudioContext)();
      const source = audioContext.createMediaElementSource(audioRef.current!);
      const analyser = audioContext.createAnalyser();
      
      // Filter chain
      const filter1 = audioContext.createBiquadFilter();
      const filter2 = audioContext.createBiquadFilter();
      
      filter1.type = 'allpass';
      filter2.type = 'allpass';
      
      source.connect(filter1);
      filter1.connect(filter2);
      filter2.connect(analyser);
      analyser.connect(audioContext.destination);
      
      analyserRef.current = analyser;
      dataArrayRef.current = new Uint8Array(analyser.frequencyBinCount);
      audioContextRef.current = audioContext;
      filterNodeRef.current = filter1;
      filterNode2Ref.current = filter2;

      const updateVisualizer = () => {
        if (analyserRef.current && dataArrayRef.current) {
          analyserRef.current.getByteFrequencyData(dataArrayRef.current);
          const newData = Array.from(dataArrayRef.current).slice(0, 40).map((v: any) => (v as number) / 255);
          setVisualizerData(newData);
        }
        animationFrameRef.current = requestAnimationFrame(updateVisualizer);
      };

      updateVisualizer();
    };

    const handleInteraction = () => {
      initAudioContext();
      if (audioContextRef.current?.state === 'suspended') {
        audioContextRef.current.resume();
      }
      window.removeEventListener('click', handleInteraction);
    };

    window.addEventListener('click', handleInteraction);

    return () => {
      if (animationFrameRef.current) cancelAnimationFrame(animationFrameRef.current);
      window.removeEventListener('click', handleInteraction);
      // We don't close the context here as it might be needed for the whole session
    };
  }, [isLoggedIn, webAudioEnabled]);

  useEffect(() => {
    localStorage.setItem('aura_audio_quality', audioQuality);
    
    // Update current stream if playing
    if (currentTrack && audioRef.current) {
      const currentTime = audioRef.current.currentTime;
      const wasPlaying = !audioRef.current.paused;
      const bitrate = audioQuality === '320kbps' ? 320 : (audioQuality === '128kbps' ? 128 : undefined);
      const streamUrl = jellyfin.getStreamUrl(currentTrack.Id, bitrate);
      
      if (audioRef.current.src !== streamUrl) {
        audioRef.current.src = streamUrl;
        audioRef.current.currentTime = currentTime;
        if (wasPlaying) audioRef.current.play();
      }
    }
  }, [audioQuality, currentTrack]);

  useEffect(() => {
    if (isDark) {
      document.body.classList.remove('light-theme');
      localStorage.setItem('aura_theme', 'dark');
    } else {
      document.body.classList.add('light-theme');
      localStorage.setItem('aura_theme', 'light');
    }
  }, [isDark]);

  // Apply Accent Color
  useEffect(() => {
    const root = document.documentElement;
    const colors = {
      purple: '263.4 70% 50.4%',
      blue: '217.2 91.2% 59.8%',
      green: '142.1 70.6% 45.3%',
      pink: '322.2 75.8% 58.4%',
      orange: '24.6 95% 53.1%'
    };
    root.style.setProperty('--primary', colors[accentColor]);
    localStorage.setItem('aura_accent_color', accentColor);
  }, [accentColor]);

  // Apply Font Family
  useEffect(() => {
    document.body.style.fontFamily = `var(--font-${fontFamily})`;
    localStorage.setItem('aura_font_family', fontFamily);
  }, [fontFamily]);

  // Sleep Timer Logic
  useEffect(() => {
    if (sleepTimer !== null && sleepTimer > 0) {
      sleepTimerRef.current = setInterval(() => {
        setSleepTimer(prev => {
          if (prev === null || prev <= 1) {
            if (audioRef.current) audioRef.current.pause();
            clearInterval(sleepTimerRef.current!);
            return null;
          }
          return prev - 1;
        });
      }, 60000); // Every minute
    } else {
      if (sleepTimerRef.current) clearInterval(sleepTimerRef.current);
    }
    return () => {
      if (sleepTimerRef.current) clearInterval(sleepTimerRef.current);
    };
  }, [sleepTimer]);

  // Auto Play on Startup
  useEffect(() => {
    if (isLoggedIn && autoPlay && tracks.length > 0 && !currentTrack) {
      playTrack(tracks[0]);
    }
  }, [isLoggedIn, autoPlay, tracks]);

  useEffect(() => {
    if (isLoggedIn) {
      loadTracks();
      loadFavorites();
      loadPlaylists();
    }
  }, [isLoggedIn]);

  // Apply Audio Filters
  useEffect(() => {
    if (!webAudioEnabled) return;
    if (!filterNodeRef.current || !filterNode2Ref.current) return;

    const f1 = filterNodeRef.current;
    const f2 = filterNode2Ref.current;

    switch (audioFilter) {
      case 'vintage':
        f1.type = 'highpass';
        f1.frequency.value = 500;
        f2.type = 'lowpass';
        f2.frequency.value = 3000;
        break;
      case 'vinyl':
        f1.type = 'lowpass';
        f1.frequency.value = 10000;
        f2.type = 'peaking';
        f2.frequency.value = 100;
        f2.gain.value = 5;
        break;
      case 'lofi':
        f1.type = 'lowpass';
        f1.frequency.value = 2000;
        f2.type = 'allpass';
        break;
      default:
        f1.type = 'allpass';
        f2.type = 'allpass';
    }
  }, [audioFilter, webAudioEnabled]);

  const getAiInsight = async () => {
    if (!currentTrack) return;
    setIsAiLoading(true);
    setAiInsight(null);
    try {
      const response = await getGeminiResponse(
        currentTrack.Name,
        currentTrack.ArtistItems[0]?.Name || 'Artista desconhecido',
      );
      setAiInsight(response || "Não consegui encontrar curiosidades sobre esta faixa.");
    } catch (error) {
      console.error('AI Error:', error);
      setAiInsight("Ocorreu um erro ao buscar informações da IA.");
    } finally {
      setIsAiLoading(false);
    }
  };

  useEffect(() => {
    if (audioRef.current) {
      audioRef.current.volume = volume;
    }
  }, [volume]);

  useEffect(() => {
    if (currentTrack) {
      document.title = `${isPlaying ? '▶' : '⏸'} ${currentTrack.Name} - Aura`;
    } else {
      document.title = 'Aura Music';
    }
  }, [currentTrack, isPlaying]);

  const loadTracks = async () => {
    setIsRefreshing(true);
    try {
      const data = await jellyfin.getTracks(100);
      setTracks(data);
    } catch (err) {
      console.error(err);
    } finally {
      setIsRefreshing(false);
    }
  };

  const loadFavorites = async () => {
    try {
      const data = await jellyfin.getFavorites();
      setFavoriteTracks(data);
    } catch (error) {
      console.error('Error loading favorites:', error);
    }
  };

  const loadPlaylists = async () => {
    try {
      const data = await jellyfin.getPlaylists();
      setPlaylists(data);
    } catch (error) {
      console.error('Error loading playlists:', error);
    }
  };

  const toggleFavorite = async (track: Track) => {
    const isFav = track.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === track.Id);
    try {
      await jellyfin.toggleFavorite(track.Id, !isFav);
      // Update local state
      const updatedTrack = { ...track, UserData: { ...track.UserData, IsFavorite: !isFav } as any };
      
      setTracks(prev => prev.map(t => t.Id === track.Id ? updatedTrack : t));
      if (currentTrack?.Id === track.Id) setCurrentTrack(updatedTrack);
      
      if (!isFav) {
        setFavoriteTracks(prev => [...prev, updatedTrack]);
      } else {
        setFavoriteTracks(prev => prev.filter(t => t.Id !== track.Id));
      }
    } catch (error) {
      console.error('Error toggling favorite:', error);
    }
  };

  const playTrack = async (track: Track) => {
    setCurrentTrack(track);
    setIsPlaying(true);
    if (audioRef.current) {
      const bitrate = audioQuality === '320kbps' ? 320 : (audioQuality === '128kbps' ? 128 : undefined);
      const streamUrl = jellyfin.getStreamUrl(track.Id, bitrate);
      // Only update src if it's a different track or quality changed
      if (audioRef.current.src !== streamUrl) {
        audioRef.current.src = streamUrl;
      }
      
      try {
        await audioRef.current.play();
      } catch (err) {
        // Ignore AbortError which happens when play() is interrupted by pause()
        if (err instanceof Error && err.name !== 'AbortError') {
          console.error("Erro ao reproduzir:", err);
        }
      }
    }
  };

  const togglePlay = async () => {
    if (!audioRef.current) return;
    
    try {
      if (audioRef.current.paused) {
        await audioRef.current.play();
      } else {
        audioRef.current.pause();
      }
    } catch (err) {
      if (err instanceof Error && err.name !== 'AbortError') {
        console.error("Erro ao alternar reprodução:", err);
      }
    }
  };

  const handleTimeUpdate = () => {
    if (audioRef.current) {
      setProgress(audioRef.current.currentTime);
      setDuration(audioRef.current.duration);
    }
  };

  const handleSeek = (e: React.ChangeEvent<HTMLInputElement>) => {
    const time = parseFloat(e.target.value);
    if (audioRef.current) {
      audioRef.current.currentTime = time;
      setProgress(time);
    }
  };

  const nextTrack = () => {
    if (!currentTrack || tracks.length === 0) return;
    
    if (isShuffle) {
      const randomIndex = Math.floor(Math.random() * tracks.length);
      playTrack(tracks[randomIndex]);
      return;
    }

    const index = tracks.findIndex(t => t.Id === currentTrack.Id);
    const next = tracks[(index + 1) % tracks.length];
    playTrack(next);
  };

  const prevTrack = () => {
    if (!currentTrack || tracks.length === 0) return;
    
    if (audioRef.current && audioRef.current.currentTime > 3) {
      audioRef.current.currentTime = 0;
      return;
    }

    const index = tracks.findIndex(t => t.Id === currentTrack.Id);
    const prev = tracks[(index - 1 + tracks.length) % tracks.length];
    playTrack(prev);
  };

  const handleEnded = () => {
    if (isRepeat) {
      if (audioRef.current) {
        audioRef.current.currentTime = 0;
        audioRef.current.play();
      }
    } else {
      nextTrack();
    }
  };

  const filteredTracks = tracks.filter(t => 
    t.Name.toLowerCase().includes(searchQuery.toLowerCase()) ||
    t.ArtistItems[0]?.Name.toLowerCase().includes(searchQuery.toLowerCase())
  );

  const albums = Array.from(new Set(tracks.map(t => t.Album))).filter(Boolean);
  const artists = Array.from(new Set(tracks.flatMap(t => t.ArtistItems.map(a => a.Name)))).filter(Boolean);

  if (isSessionLoading) {
    return (
      <div className="fixed inset-0 flex items-center justify-center bg-[#050505]">
        <div className="flex items-center gap-3 text-white/60">
          <Loader2 className="w-5 h-5 animate-spin" />
          <span className="text-sm uppercase tracking-[0.3em]">Carregando sessão</span>
        </div>
      </div>
    );
  }

  if (!isLoggedIn) {
    return (
      <LoginScreen
        onLogin={(config) => {
          setSessionConfig(config);
          setIsLoggedIn(true);
        }}
        isDark={isDark}
        onToggleTheme={() => setIsDark(!isDark)}
        bgAnimationEnabled={bgAnimationEnabled}
        bgAnimationSpeed={bgAnimationSpeed}
      />
    );
  }

  return (
    <div className="flex h-screen overflow-hidden bg-transparent font-sans relative">
      <SynesthesiaBackground data={visualizerData} accentColor={accentColor} enabled={synesthesiaEnabled} />
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
      <audio 
        ref={audioRef} 
        onTimeUpdate={handleTimeUpdate} 
        onEnded={handleEnded}
        onPlay={() => setIsPlaying(true)}
        onPause={() => setIsPlaying(false)}
        onError={() => {
          console.error('Audio element error:', audioRef.current?.error, audioRef.current?.src);
        }}
        crossOrigin="anonymous"
      />

      {/* Sidebar */}
      <aside className={cn(
        "w-64 glass-dark flex flex-col p-6 pb-32 z-20 border-r border-white/5 transition-all duration-500",
        immersiveMode && !isSettingsOpen ? "-translate-x-full opacity-0 w-0 p-0 overflow-hidden" : "translate-x-0 opacity-100"
      )}>
        <div className="flex items-center gap-3 mb-10 px-2">
          <div className="w-10 h-10 bg-gradient-to-tr from-purple-600 to-blue-500 rounded-xl flex items-center justify-center shadow-lg shadow-purple-500/20">
            <Music className="text-white w-5 h-5" />
          </div>
          <span className="text-xl font-display font-bold tracking-tight">MyFin</span>
        </div>

        <nav className="flex-1 space-y-1 overflow-y-auto custom-scrollbar pr-2">
          <NavItem 
            icon={<LayoutGrid className="w-4 h-4" />} 
            label="Explorar" 
            active={activeView === 'explore'} 
            onClick={() => setActiveView('explore')}
          />
          <NavItem 
            icon={<Disc className="w-4 h-4" />} 
            label="Álbuns" 
            active={activeView === 'albums'} 
            onClick={() => setActiveView('albums')}
          />
          <NavItem 
            icon={<User className="w-4 h-4" />} 
            label="Artistas" 
            active={activeView === 'artists'} 
            onClick={() => setActiveView('artists')}
          />
          <NavItem 
            icon={<Heart className="w-4 h-4" />} 
            label="Favoritos" 
            active={activeView === 'favorites'} 
            onClick={() => setActiveView('favorites')}
          />
          <NavItem 
            icon={<ListMusic className="w-4 h-4" />} 
            label="Playlists" 
            active={activeView === 'playlists'} 
            onClick={() => setActiveView('playlists')}
          />
        </nav>

        <div className="mt-auto pt-6 border-t border-white/5">
          <NavItem 
            icon={<Settings className="w-4 h-4" />} 
            label="Configurações" 
            onClick={() => setIsSettingsOpen(true)}
          />
        </div>
      </aside>

      {/* Main Content */}
      <main className="flex-1 flex flex-col relative overflow-hidden">
        {/* Header */}
        <header className="p-8 flex items-center justify-between z-10">
          <div className="relative w-96 group">
            <Search className="absolute left-4 top-1/2 -translate-y-1/2 w-4 h-4 text-white/30 group-focus-within:text-purple-400 transition-colors" />
            <input 
              type="text" 
              placeholder="Buscar músicas, álbuns..."
              className="w-full bg-white/5 border border-white/10 rounded-full py-2.5 pl-12 pr-4 focus:outline-none focus:ring-2 focus:ring-purple-500/30 transition-all text-sm hover:bg-white/10"
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
            />
          </div>
          <div className="flex items-center gap-6">
            <ThemeSwitch isDark={isDark} onToggle={() => setIsDark(!isDark)} />
            <button 
              onClick={loadTracks}
              disabled={isRefreshing}
              className={cn(
                "p-2.5 rounded-full glass hover:bg-white/10 transition-all active:scale-90",
                isRefreshing && "animate-spin text-purple-400"
              )}
            >
              <Disc className="w-4 h-4" />
            </button>
            <div className="flex items-center gap-4">
              <div className="text-right hidden sm:block">
                <p className="text-[10px] font-bold text-white/30 uppercase tracking-[0.2em]">Servidor</p>
                <p className="text-sm font-semibold text-white/80">Online</p>
              </div>
              <div className="w-10 h-10 rounded-2xl bg-gradient-to-br from-white/10 to-white/5 border border-white/10 flex items-center justify-center shadow-inner">
                <User className="w-5 h-5 text-white/60" />
              </div>
            </div>
          </div>
        </header>

        {/* Scrollable Area */}
        <div className="flex-1 overflow-y-auto px-8 pb-40 custom-scrollbar">
          <AnimatePresence mode="wait">
            {activeView === 'explore' && (
              <motion.div
                key="explore"
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
              >
                <div className="mb-10">
                  <h2 className="text-5xl font-display font-bold mb-3 tracking-tight">Sua Biblioteca</h2>
                  <p className="text-white/40 text-lg">Descubra e ouça suas músicas favoritas do Jellyfin.</p>
                </div>

                <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 2xl:grid-cols-6 gap-8">
                  {filteredTracks.map((track, index) => (
                    <TrackCard 
                      key={track.Id} 
                      track={track} 
                      index={index} 
                      onPlay={playTrack} 
                      albumArtStyle={albumArtStyle}
                      onToggleFavorite={toggleFavorite}
                      isFavorite={track.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === track.Id)}
                    />
                  ))}
                </div>
              </motion.div>
            )}

            {activeView === 'albums' && (
              <motion.div
                key="albums"
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
              >
                <div className="mb-10">
                  <h2 className="text-5xl font-display font-bold mb-3 tracking-tight">Álbuns</h2>
                  <p className="text-white/40 text-lg">{albums.length} álbuns encontrados na sua biblioteca.</p>
                </div>
                <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 gap-8">
                  {albums.map((album, index) => (
                    <div key={album} className="glass p-4 rounded-3xl hover:bg-white/5 transition-all cursor-pointer group">
                      <div className="aspect-square rounded-2xl bg-white/5 mb-4 flex items-center justify-center overflow-hidden">
                        <Disc className="w-12 h-12 text-white/10 group-hover:scale-110 group-hover:rotate-12 transition-transform duration-500" />
                      </div>
                      <h3 className="font-bold truncate">{album}</h3>
                      <p className="text-xs text-white/40 mt-1">Álbum</p>
                    </div>
                  ))}
                </div>
              </motion.div>
            )}

            {activeView === 'artists' && (
              <motion.div
                key="artists"
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
              >
                <div className="mb-10">
                  <h2 className="text-5xl font-display font-bold mb-3 tracking-tight">Artistas</h2>
                  <p className="text-white/40 text-lg">{artists.length} artistas na sua coleção.</p>
                </div>
                <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 gap-8">
                  {artists.map((artist, index) => (
                    <div key={artist} className="glass p-6 rounded-full flex flex-col items-center text-center hover:bg-white/5 transition-all cursor-pointer group">
                      <div className="w-24 h-24 rounded-full bg-gradient-to-tr from-purple-500/20 to-blue-500/20 mb-4 flex items-center justify-center overflow-hidden border border-white/5">
                        <User className="w-10 h-10 text-white/20 group-hover:scale-110 transition-transform duration-500" />
                      </div>
                      <h3 className="font-bold truncate w-full">{artist}</h3>
                    </div>
                  ))}
                </div>
              </motion.div>
            )}

            {activeView === 'favorites' && (
              <motion.div
                key="favorites"
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                className="space-y-12"
              >
                <div className="flex items-center justify-between">
                  <div>
                    <h2 className="text-5xl font-display font-bold tracking-tighter mb-3">Favoritos</h2>
                    <p className="text-white/40 font-medium uppercase tracking-widest text-xs">Suas músicas curtidas no Jellyfin</p>
                  </div>
                </div>

                {favoriteTracks.length > 0 ? (
                  <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 2xl:grid-cols-6 gap-8">
                    {favoriteTracks.map((track, index) => (
                      <TrackCard 
                        key={track.Id} 
                        track={track} 
                        index={index} 
                        onPlay={playTrack} 
                        albumArtStyle={albumArtStyle}
                        onToggleFavorite={toggleFavorite}
                        isFavorite={true}
                      />
                    ))}
                  </div>
                ) : (
                  <div className="flex flex-col items-center justify-center h-[40vh] text-center">
                    <div className="w-20 h-20 rounded-3xl glass flex items-center justify-center mb-6">
                      <Heart className="w-8 h-8 text-white/20" />
                    </div>
                    <h3 className="text-2xl font-bold mb-2">Nenhum favorito</h3>
                    <p className="text-white/40 max-w-xs">Curta algumas músicas para vê-las aqui.</p>
                  </div>
                )}
              </motion.div>
            )}

            {activeView === 'playlists' && (
              <motion.div
                key="playlists"
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                className="space-y-12"
              >
                <div className="flex items-center justify-between">
                  <div>
                    <h2 className="text-5xl font-display font-bold tracking-tighter mb-3">Playlists</h2>
                    <p className="text-white/40 font-medium uppercase tracking-widest text-xs">Suas coleções personalizadas</p>
                  </div>
                </div>

                {playlists.length > 0 ? (
                  <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 2xl:grid-cols-6 gap-8">
                    {playlists.map((playlist, index) => (
                      <motion.div
                        key={playlist.Id}
                        initial={{ opacity: 0, y: 20 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ delay: index * 0.05 }}
                        className="group cursor-pointer"
                        onClick={async () => {
                          const items = await jellyfin.getPlaylistItems(playlist.Id);
                          setTracks(items);
                          setActiveView('explore');
                        }}
                      >
                        <div className="aspect-square glass rounded-[32px] flex items-center justify-center group-hover:scale-105 transition-all duration-500 shadow-2xl overflow-hidden relative">
                          <div className="absolute inset-0 bg-gradient-to-br from-purple-600/20 to-blue-500/20" />
                          <ListMusic className="w-16 h-16 text-white/20 group-hover:text-white/40 transition-colors" />
                        </div>
                        <div className="mt-5 px-3">
                          <h3 className="font-bold truncate text-sm tracking-tight">{playlist.Name}</h3>
                          <p className="text-[11px] font-medium text-white/30 mt-1 uppercase tracking-wider">{playlist.ItemCount} Músicas</p>
                        </div>
                      </motion.div>
                    ))}
                  </div>
                ) : (
                  <div className="flex flex-col items-center justify-center h-[40vh] text-center">
                    <div className="w-20 h-20 rounded-3xl glass flex items-center justify-center mb-6">
                      <ListMusic className="w-8 h-8 text-white/20" />
                    </div>
                    <h3 className="text-2xl font-bold mb-2">Nenhuma playlist</h3>
                    <p className="text-white/40 max-w-xs">Crie playlists no seu Jellyfin para vê-las aqui.</p>
                  </div>
                )}
              </motion.div>
            )}
          </AnimatePresence>
        </div>

        {/* Settings Modal */}
        <AnimatePresence>
          {isSettingsOpen && (
            <div className="fixed inset-0 z-50 flex items-center justify-center p-6">
              <motion.div 
                initial={{ opacity: 0 }}
                animate={{ opacity: 1 }}
                exit={{ opacity: 0 }}
                onClick={() => setIsSettingsOpen(false)}
                className="absolute inset-0 bg-black/60 backdrop-blur-md"
              />
              <motion.div 
                initial={{ opacity: 0, scale: 0.9, y: 20 }}
                animate={{ opacity: 1, scale: 1, y: 0 }}
                exit={{ opacity: 0, scale: 0.9, y: 20 }}
                className="w-full max-w-xl glass rounded-[40px] p-10 relative z-10 border border-white/10 shadow-2xl"
              >
                <div className="flex items-center justify-between mb-8">
                  <h2 className="text-3xl font-display font-bold">Configurações</h2>
                  <button 
                    onClick={() => setIsSettingsOpen(false)}
                    className="p-3 hover:bg-white/10 rounded-2xl transition-all active:scale-90"
                  >
                    <X className="w-6 h-6" />
                  </button>
                </div>

                {/* Tabs */}
                <div className="flex gap-2 mb-8 p-1.5 glass rounded-2xl relative overflow-x-auto custom-scrollbar no-scrollbar">
                  {(['interface', 'audio', 'soundlab', 'advanced', 'account'] as const).map((tab) => (
                    <button 
                      key={tab}
                      onClick={() => setSettingsTab(tab)}
                      className={cn(
                        "flex-1 min-w-[100px] py-3 rounded-xl text-sm font-bold transition-all flex items-center justify-center gap-2 relative z-10",
                        settingsTab === tab ? "text-white" : "text-white/40 hover:text-white/60"
                      )}
                    >
                      {tab === 'interface' && <LayoutGrid className="w-4 h-4" />}
                      {tab === 'audio' && <Volume2 className="w-4 h-4" />}
                      {tab === 'soundlab' && <Zap className="w-4 h-4" />}
                      {tab === 'advanced' && <Activity className="w-4 h-4" />}
                      {tab === 'account' && <User className="w-4 h-4" />}
                      <span className="capitalize">
                        {tab === 'audio' ? 'Áudio' : tab === 'account' ? 'Conta' : tab === 'advanced' ? 'Avançado' : tab === 'soundlab' ? 'Sound Lab' : tab}
                      </span>
                      {settingsTab === tab && (
                        <motion.div 
                          layoutId="settings-tab-active"
                          className="absolute inset-0 bg-white/10 rounded-xl shadow-xl border border-white/5 -z-10"
                          transition={{ type: "spring", bounce: 0.15, duration: 0.5 }}
                        />
                      )}
                    </button>
                  ))}
                </div>

                <div className="min-h-[320px]">
                  <AnimatePresence mode="wait">
                    {settingsTab === 'interface' && (
                      <motion.div 
                        key="interface"
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        exit={{ opacity: 0, y: -10 }}
                        transition={{ duration: 0.2 }}
                        className="space-y-6 max-h-[450px] overflow-y-auto pr-2 custom-scrollbar"
                      >
                        <div className="space-y-4">
                          <h3 className="text-xs font-bold text-white/30 uppercase tracking-widest">Visual</h3>
                          
                          {/* Accent Color */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-purple-500/20 flex items-center justify-center text-purple-400">
                                <Palette className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Cor de Destaque</span>
                                <span className="text-xs text-white/40">Personalize a cor principal do app</span>
                              </div>
                            </div>
                            <div className="flex gap-3">
                              {(['purple', 'blue', 'green', 'pink', 'orange'] as const).map((color) => (
                                <button 
                                  key={color}
                                  onClick={() => setAccentColor(color)}
                                  className={cn(
                                    "w-8 h-8 rounded-full transition-all border-2",
                                    color === 'purple' && "bg-purple-500",
                                    color === 'blue' && "bg-blue-500",
                                    color === 'green' && "bg-green-500",
                                    color === 'pink' && "bg-pink-500",
                                    color === 'orange' && "bg-orange-500",
                                    accentColor === color ? "border-white scale-125 shadow-lg" : "border-transparent opacity-60 hover:opacity-100"
                                  )}
                                />
                              ))}
                            </div>
                          </div>

                          {/* Background Animation */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center justify-between">
                              <div className="flex items-center gap-4">
                                <div className={cn(
                                  "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                  bgAnimationEnabled ? "bg-blue-500/20 text-blue-400" : "bg-white/5 text-white/20"
                                )}>
                                  <Sparkles className="w-5 h-5" />
                                </div>
                                <div className="text-left">
                                  <span className="font-bold block">Animação de Fundo</span>
                                  <span className="text-xs text-white/40">Anéis neon giratórios no fundo</span>
                                </div>
                              </div>
                              <button 
                                onClick={() => setBgAnimationEnabled(!bgAnimationEnabled)}
                                className={cn(
                                  "w-12 h-6 rounded-full relative transition-colors duration-300",
                                  bgAnimationEnabled ? "bg-blue-600" : "bg-white/10"
                                )}
                              >
                                <motion.div 
                                  animate={{ x: bgAnimationEnabled ? 24 : 4 }}
                                  className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                                />
                              </button>
                            </div>
                            {bgAnimationEnabled && (
                              <div className="flex gap-2 pt-2">
                                {(['slow', 'normal', 'fast'] as const).map((speed) => (
                                  <button 
                                    key={speed}
                                    onClick={() => setBgAnimationSpeed(speed)}
                                    className={cn(
                                      "flex-1 py-2 rounded-xl text-xs font-bold transition-all border",
                                      speed === bgAnimationSpeed 
                                        ? "bg-blue-500 border-blue-400 text-white" 
                                        : "bg-white/5 border-white/5 text-white/40"
                                    )}
                                  >
                                    {speed === 'slow' ? 'Lenta' : speed === 'normal' ? 'Normal' : 'Rápida'}
                                  </button>
                                ))}
                              </div>
                            )}
                          </div>

                          {/* Album Art Style */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-orange-500/20 flex items-center justify-center text-orange-400">
                                <ImageIcon className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Estilo das Capas</span>
                                <span className="text-xs text-white/40">Formato das artes dos álbuns</span>
                              </div>
                            </div>
                            <div className="flex gap-3">
                              {(['square', 'round'] as const).map((style) => (
                                <button 
                                  key={style}
                                  onClick={() => setAlbumArtStyle(style)}
                                  className={cn(
                                    "flex-1 py-3 rounded-xl text-xs font-bold transition-all border flex items-center justify-center gap-2",
                                    style === albumArtStyle 
                                      ? "bg-orange-500 border-orange-400 text-white" 
                                      : "bg-white/5 border-white/5 text-white/40 hover:bg-white/10"
                                  )}
                                >
                                  {style === 'square' ? <LayoutGrid className="w-4 h-4" /> : <Disc className="w-4 h-4" />}
                                  {style === 'square' ? 'Quadrado' : 'Vinil (Redondo)'}
                                </button>
                              ))}
                            </div>
                          </div>

                          {/* Font Family */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-green-500/20 flex items-center justify-center text-green-400">
                                <Type className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Tipografia</span>
                                <span className="text-xs text-white/40">Escolha a fonte do aplicativo</span>
                              </div>
                            </div>
                            <div className="flex gap-3">
                              {(['sans', 'display', 'serif'] as const).map((font) => (
                                <button 
                                  key={font}
                                  onClick={() => setFontFamily(font)}
                                  className={cn(
                                    "flex-1 py-3 rounded-xl text-xs font-bold transition-all border",
                                    font === fontFamily 
                                      ? "bg-green-500 border-green-400 text-white" 
                                      : "bg-white/5 border-white/5 text-white/40 hover:bg-white/10"
                                  )}
                                  style={{ fontFamily: font === 'sans' ? 'Inter' : font === 'display' ? 'Space Grotesk' : 'Playfair Display' }}
                                >
                                  {font === 'sans' ? 'Sans' : font === 'display' ? 'Modern' : 'Serif'}
                                </button>
                              ))}
                            </div>
                          </div>

                          {/* Language */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-blue-500/20 flex items-center justify-center text-blue-400">
                                <Languages className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Idioma</span>
                                <span className="text-xs text-white/40">Selecione o idioma da interface</span>
                              </div>
                            </div>
                            <div className="flex gap-3">
                              {(['pt', 'en', 'es'] as const).map((lang) => (
                                <button 
                                  key={lang}
                                  onClick={() => setLanguage(lang)}
                                  className={cn(
                                    "flex-1 py-3 rounded-xl text-xs font-bold transition-all border",
                                    lang === language 
                                      ? "bg-blue-500 border-blue-400 text-white" 
                                      : "bg-white/5 border-white/5 text-white/40 hover:bg-white/10"
                                  )}
                                >
                                  {lang === 'pt' ? 'Português' : lang === 'en' ? 'English' : 'Español'}
                                </button>
                              ))}
                            </div>
                          </div>

                          <button 
                            onClick={() => setImmersiveMode(!immersiveMode)}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                          >
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                immersiveMode ? "bg-purple-500/20 text-purple-400" : "bg-white/5 text-white/20"
                              )}>
                                <LayoutGrid className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Modo Imersivo</span>
                                <span className="text-xs text-white/40">Esconde a barra lateral automaticamente</span>
                              </div>
                            </div>
                            <div className={cn(
                              "w-12 h-6 rounded-full relative transition-colors duration-300",
                              immersiveMode ? "bg-purple-600" : "bg-white/10"
                            )}>
                              <motion.div 
                                animate={{ x: immersiveMode ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </div>
                          </button>
                        </div>
                      </motion.div>
                    )}

                    {settingsTab === 'audio' && (
                      <motion.div 
                        key="audio"
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        exit={{ opacity: 0, y: -10 }}
                        transition={{ duration: 0.2 }}
                        className="space-y-6 max-h-[450px] overflow-y-auto pr-2 custom-scrollbar"
                      >
                        <div className="space-y-4">
                          <h3 className="text-xs font-bold text-white/30 uppercase tracking-widest">Processamento</h3>
                          
                          {/* Equalizer */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-purple-500/20 flex items-center justify-center text-purple-400">
                                <Activity className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Equalizador</span>
                                <span className="text-xs text-white/40">Ajuste o perfil sonoro</span>
                              </div>
                            </div>
                            <div className="grid grid-cols-3 gap-2">
                              {(['none', 'bass', 'rock', 'pop', 'vocal'] as const).map((eq) => (
                                <button 
                                  key={eq}
                                  onClick={() => setEqualizerPreset(eq)}
                                  className={cn(
                                    "py-2 rounded-xl text-[10px] font-bold transition-all border uppercase tracking-wider",
                                    eq === equalizerPreset 
                                      ? "bg-purple-500 border-purple-400 text-white" 
                                      : "bg-white/5 border-white/5 text-white/40 hover:bg-white/10"
                                  )}
                                >
                                  {eq === 'none' ? 'Normal' : eq}
                                </button>
                              ))}
                            </div>
                          </div>

                          {/* Crossfade */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center justify-between">
                              <div className="flex items-center gap-4">
                                <div className="w-10 h-10 rounded-xl bg-blue-500/20 flex items-center justify-center text-blue-400">
                                  <Shuffle className="w-5 h-5" />
                                </div>
                                <div className="text-left">
                                  <span className="font-bold block">Crossfade</span>
                                  <span className="text-xs text-white/40">Transição suave entre faixas</span>
                                </div>
                              </div>
                              <span className="text-sm font-mono font-bold text-blue-400">{crossfadeTime}s</span>
                            </div>
                            <input 
                              type="range" 
                              min="0" 
                              max="10" 
                              step="1"
                              value={crossfadeTime}
                              onChange={(e) => setCrossfadeTime(parseInt(e.target.value))}
                              className="w-full h-1.5 bg-white/10 rounded-full appearance-none cursor-pointer accent-blue-500"
                            />
                          </div>

                          {/* Sleep Timer */}
                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center justify-between">
                              <div className="flex items-center gap-4">
                                <div className={cn(
                                  "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                  sleepTimer !== null ? "bg-orange-500/20 text-orange-400" : "bg-white/5 text-white/20"
                                )}>
                                  <Clock className="w-5 h-5" />
                                </div>
                                <div className="text-left">
                                  <span className="font-bold block">Temporizador</span>
                                  <span className="text-xs text-white/40">Desligar música automaticamente</span>
                                </div>
                              </div>
                              {sleepTimer !== null && (
                                <span className="text-sm font-mono font-bold text-orange-400">{sleepTimer}m</span>
                              )}
                            </div>
                            <div className="flex gap-2">
                              {[15, 30, 60].map((m) => (
                                <button 
                                  key={m}
                                  onClick={() => setSleepTimer(sleepTimer === m ? null : m)}
                                  className={cn(
                                    "flex-1 py-2 rounded-xl text-xs font-bold transition-all border",
                                    sleepTimer === m 
                                      ? "bg-orange-500 border-orange-400 text-white" 
                                      : "bg-white/5 border-white/5 text-white/40"
                                  )}
                                >
                                  {m}m
                                </button>
                              ))}
                              <button 
                                onClick={() => setSleepTimer(null)}
                                className="p-2 glass rounded-xl text-white/40 hover:text-red-500"
                              >
                                <X className="w-4 h-4" />
                              </button>
                            </div>
                          </div>

                          {/* Auto Play */}
                          <button 
                            onClick={() => setAutoPlay(!autoPlay)}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                          >
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                autoPlay ? "bg-green-500/20 text-green-400" : "bg-white/5 text-white/20"
                              )}>
                                <PlayCircle className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Reprodução Automática</span>
                                <span className="text-xs text-white/40">Começar a tocar ao abrir o app</span>
                              </div>
                            </div>
                            <div className={cn(
                              "w-12 h-6 rounded-full relative transition-colors duration-300",
                              autoPlay ? "bg-green-600" : "bg-white/10"
                            )}>
                              <motion.div 
                                animate={{ x: autoPlay ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </div>
                          </button>

                          <button 
                            onClick={() => setVolumeNormalization(!volumeNormalization)}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                          >
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                volumeNormalization ? "bg-blue-500/20 text-blue-400" : "bg-white/5 text-white/20"
                              )}>
                                <Volume2 className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Normalização</span>
                                <span className="text-xs text-white/40">Mantém o volume constante entre faixas</span>
                              </div>
                            </div>
                            <div className={cn(
                              "w-12 h-6 rounded-full relative transition-colors duration-300",
                              volumeNormalization ? "bg-blue-600" : "bg-white/10"
                            )}>
                              <motion.div 
                                animate={{ x: volumeNormalization ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </div>
                          </button>

                          <div className="p-5 glass rounded-2xl space-y-4">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-blue-500/20 flex items-center justify-center text-blue-400">
                                <Disc className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Qualidade de Áudio</span>
                                <span className="text-xs text-white/40">Ajuste o bitrate do streaming</span>
                              </div>
                            </div>
                            <div className="flex gap-2">
                              {['Auto', '320kbps', '128kbps'].map((q) => (
                                <button 
                                  key={q}
                                  onClick={() => setAudioQuality(q as any)}
                                  className={cn(
                                    "flex-1 py-2.5 rounded-xl text-xs font-bold transition-all border",
                                    q === audioQuality 
                                      ? "bg-blue-500 border-blue-400 text-white shadow-lg shadow-blue-500/20" 
                                      : "bg-white/5 border-white/5 text-white/40 hover:bg-white/10"
                                  )}
                                >
                                  {q}
                                </button>
                              ))}
                            </div>
                          </div>
                        </div>
                      </motion.div>
                    )}

                    {settingsTab === 'soundlab' && (
                      <motion.div 
                        key="soundlab"
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        exit={{ opacity: 0, y: -10 }}
                        transition={{ duration: 0.2 }}
                        className="space-y-8"
                      >
                        <div>
                          <h3 className="text-lg font-bold mb-4 flex items-center gap-2">
                            <Zap className="w-5 h-5 text-yellow-400" />
                            Laboratório de Som (Filtros de Era)
                          </h3>
                          <p className="text-sm text-white/40 mb-6">Transforme a textura do áudio para simular diferentes épocas.</p>
                          
                          <div className="grid grid-cols-2 gap-4">
                            {[
                              { id: 'none', name: 'Original', desc: 'Som puro e cristalino', icon: <Music className="w-5 h-5" /> },
                              { id: 'vintage', name: 'Rádio 1940', desc: 'Som de rádio antigo', icon: <Activity className="w-5 h-5" /> },
                              { id: 'vinyl', name: 'Vinil Quente', desc: 'Graves ricos e calor', icon: <Disc className="w-5 h-5" /> },
                              { id: 'lofi', name: 'Lo-Fi Muffled', desc: 'Som abafado e relaxante', icon: <Clock className="w-5 h-5" /> },
                            ].map((filter) => (
                              <button
                                key={filter.id}
                                onClick={() => setAudioFilter(filter.id as any)}
                                className={cn(
                                  "p-5 rounded-3xl border-2 text-left transition-all group",
                                  audioFilter === filter.id 
                                    ? "border-primary bg-primary/10" 
                                    : "border-white/5 bg-white/5 hover:border-white/20"
                                )}
                              >
                                <div className={cn(
                                  "w-10 h-10 rounded-xl flex items-center justify-center mb-4 transition-all",
                                  audioFilter === filter.id ? "bg-primary text-white" : "bg-white/10 text-white/40 group-hover:text-white"
                                )}>
                                  {filter.icon}
                                </div>
                                <h4 className="font-bold mb-1">{filter.name}</h4>
                                <p className="text-[10px] text-white/40 uppercase tracking-widest">{filter.desc}</p>
                              </button>
                            ))}
                          </div>
                        </div>

                        <div className="p-5 glass rounded-2xl space-y-4">
                          <div className="flex items-center justify-between">
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                synesthesiaEnabled ? "bg-purple-500/20 text-purple-400" : "bg-white/5 text-white/20"
                              )}>
                                <Activity className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Sinestesia Visual</span>
                                <span className="text-xs text-white/40">Fundo reativo à energia da música</span>
                              </div>
                            </div>
                            <button 
                              onClick={() => {
                                const newVal = !synesthesiaEnabled;
                                setSynesthesiaEnabled(newVal);
                                localStorage.setItem('aura_synesthesia_enabled', newVal.toString());
                              }}
                              className={cn(
                                "w-12 h-6 rounded-full relative transition-colors duration-300",
                                synesthesiaEnabled ? "bg-purple-600" : "bg-white/10"
                              )}
                            >
                              <motion.div 
                                animate={{ x: synesthesiaEnabled ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </button>
                          </div>
                        </div>
                      </motion.div>
                    )}
                    {settingsTab === 'advanced' && (
                      <motion.div 
                        key="advanced"
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        exit={{ opacity: 0, y: -10 }}
                        transition={{ duration: 0.2 }}
                        className="space-y-6"
                      >
                        <div className="space-y-4">
                          <h3 className="text-xs font-bold text-white/30 uppercase tracking-widest">Conectividade</h3>
                          
                          {/* Notifications */}
                          <button 
                            onClick={() => setNotificationsEnabled(!notificationsEnabled)}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                          >
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                notificationsEnabled ? "bg-purple-500/20 text-purple-400" : "bg-white/5 text-white/20"
                              )}>
                                <Bell className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Notificações</span>
                                <span className="text-xs text-white/40">Avisos do sistema ao mudar de música</span>
                              </div>
                            </div>
                            <div className={cn(
                              "w-12 h-6 rounded-full relative transition-colors duration-300",
                              notificationsEnabled ? "bg-purple-600" : "bg-white/10"
                            )}>
                              <motion.div 
                                animate={{ x: notificationsEnabled ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </div>
                          </button>

                          {/* Last.fm */}
                          <button 
                            onClick={() => setLastfmEnabled(!lastfmEnabled)}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                          >
                            <div className="flex items-center gap-4">
                              <div className={cn(
                                "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                                lastfmEnabled ? "bg-red-500/20 text-red-400" : "bg-white/5 text-white/20"
                              )}>
                                <Activity className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Integração Last.fm</span>
                                <span className="text-xs text-white/40">Registrar músicas ouvidas (Scrobbling)</span>
                              </div>
                            </div>
                            <div className={cn(
                              "w-12 h-6 rounded-full relative transition-colors duration-300",
                              lastfmEnabled ? "bg-red-600" : "bg-white/10"
                            )}>
                              <motion.div 
                                animate={{ x: lastfmEnabled ? 24 : 4 }}
                                className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                              />
                            </div>
                          </button>

                          {/* Lyrics Placeholder */}
                          <div className="p-5 glass rounded-2xl border border-white/5 opacity-50">
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-white/5 flex items-center justify-center text-white/20">
                                <MessageSquare className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block">Letras de Música</span>
                                <span className="text-xs text-white/40">Em breve: Sincronização em tempo real</span>
                              </div>
                            </div>
                          </div>
                        </div>
                      </motion.div>
                    )}

                    {settingsTab === 'account' && (
                      <motion.div 
                        key="account"
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        exit={{ opacity: 0, y: -10 }}
                        transition={{ duration: 0.2 }}
                        className="space-y-6"
                      >
                        <div className="space-y-4">
                          <h3 className="text-xs font-bold text-white/30 uppercase tracking-widest">Sessão Atual</h3>
                          <div className="p-6 glass rounded-3xl flex items-center gap-5">
                            <div className="w-16 h-16 rounded-2xl bg-gradient-to-br from-purple-500 to-blue-500 flex items-center justify-center shadow-xl shadow-purple-500/20">
                              <User className="w-8 h-8 text-white" />
                            </div>
                            <div className="flex-1">
                              <h4 className="text-xl font-bold">{sessionConfig?.username}</h4>
                              <p className="text-sm text-white/40">Conectado ao Jellyfin</p>
                            </div>
                          </div>

                          <button 
                            onClick={async () => {
                              await jellyfin.logout();
                              setSessionConfig(null);
                              setIsLoggedIn(false);
                              setIsSettingsOpen(false);
                            }}
                            className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-red-500/10 transition-all group border border-transparent hover:border-red-500/20"
                          >
                            <div className="flex items-center gap-4">
                              <div className="w-10 h-10 rounded-xl bg-red-500/10 flex items-center justify-center text-red-500">
                                <LogOut className="w-5 h-5" />
                              </div>
                              <div className="text-left">
                                <span className="font-bold block text-red-500">Sair da Conta</span>
                                <span className="text-xs text-white/40">Desconectar deste servidor</span>
                              </div>
                            </div>
                            <ChevronRight className="w-5 h-5 text-white/20 group-hover:text-red-500 transition-colors" />
                          </button>
                        </div>
                      </motion.div>
                    )}
                  </AnimatePresence>
                </div>

                <div className="mt-8 pt-6 border-t border-white/5">
                  <div className="flex items-center justify-between text-xs mb-6 px-2">
                    <div className="flex items-center gap-2 text-white/40">
                      <Server className="w-3 h-3" />
                      <span>Servidor Jellyfin</span>
                    </div>
                    <span className="font-mono text-purple-400 font-medium">
                      {sessionConfig?.serverUrl.replace('https://', '').replace('http://', '')}
                    </span>
                  </div>

                  <button 
                    onClick={() => {
                      setIsSaving(true);
                      localStorage.setItem('aura_immersive_mode', immersiveMode.toString());
                      localStorage.setItem('aura_volume_normalization', volumeNormalization.toString());
                      localStorage.setItem('aura_bg_anim_enabled', bgAnimationEnabled.toString());
                      localStorage.setItem('aura_bg_anim_speed', bgAnimationSpeed);
                      localStorage.setItem('aura_album_art_style', albumArtStyle);
                      localStorage.setItem('aura_eq_preset', equalizerPreset);
                      localStorage.setItem('aura_crossfade_time', crossfadeTime.toString());
                      localStorage.setItem('aura_auto_play', autoPlay.toString());
                      localStorage.setItem('aura_notifications', notificationsEnabled.toString());
                      localStorage.setItem('aura_lastfm', lastfmEnabled.toString());
                      localStorage.setItem('aura_language', language);
                      
                      setTimeout(() => {
                        setIsSaving(false);
                        setSaveSuccess(true);
                        setTimeout(() => {
                          setSaveSuccess(false);
                          setIsSettingsOpen(false);
                        }, 800);
                      }, 600);
                    }}
                    disabled={isSaving || saveSuccess}
                    className={cn(
                      "w-full py-4 rounded-2xl font-bold flex items-center justify-center gap-3 transition-all active:scale-[0.98] relative overflow-hidden shadow-2xl",
                      saveSuccess 
                        ? "bg-green-500 text-white" 
                        : isSaving
                          ? "bg-white/10 text-white/40 cursor-wait"
                          : "bg-white text-black hover:bg-white/90 hover:scale-[1.02]"
                    )}
                  >
                    {isSaving ? (
                      <motion.div 
                        animate={{ rotate: 360 }}
                        transition={{ repeat: Infinity, duration: 1, ease: "linear" }}
                      >
                        <Loader2 className="w-5 h-5" />
                      </motion.div>
                    ) : saveSuccess ? (
                      <Check className="w-5 h-5" />
                    ) : (
                      <Save className="w-5 h-5" />
                    )}
                    <span>{isSaving ? 'Salvando...' : saveSuccess ? 'Configurações Salvas' : 'Salvar Alterações'}</span>
                  </button>
                </div>
              </motion.div>
            </div>
          )}
        </AnimatePresence>

        {/* Player Bar */}
        <motion.div 
          initial={{ y: 100 }}
          animate={{ y: 0 }}
          className="fixed bottom-0 left-0 right-0 h-28 glass-dark border-t border-white/5 px-8 flex items-center justify-between z-40"
        >
          {/* Track Info */}
          <div 
            className="flex items-center gap-4 w-1/3 cursor-pointer group hover:bg-white/5 p-2 -m-2 rounded-2xl transition-all"
            onClick={() => setIsNowPlayingOpen(true)}
          >
            <div className={cn(
              "w-16 h-16 overflow-hidden glass relative shadow-lg group-hover:scale-105 transition-transform duration-500",
              albumArtStyle === 'round' ? "rounded-full animate-spin-slow" : "rounded-2xl"
            )}>
              {currentTrack ? (
                <img 
                  src={jellyfin.getAlbumArtUrl(currentTrack.Id)} 
                  alt={currentTrack.Name}
                  className="w-full h-full object-cover"
                  referrerPolicy="no-referrer"
                />
              ) : (
                <div className="w-full h-full bg-white/5 flex items-center justify-center">
                  <Music className="w-6 h-6 text-white/20" />
                </div>
              )}
            </div>
            <div className="flex-1 min-w-0">
              <h4 className="font-bold truncate group-hover:text-purple-400 transition-colors">
                {currentTrack?.Name || 'Nenhuma música'}
              </h4>
              <p className="text-xs text-white/40 truncate mt-1">
                {currentTrack?.ArtistItems[0]?.Name || 'Selecione uma faixa'}
              </p>
            </div>
            <button 
              onClick={() => currentTrack && toggleFavorite(currentTrack)}
              className={cn(
                "p-2 transition-colors",
                currentTrack && (currentTrack.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === currentTrack.Id)) ? "text-red-500" : "text-white/20 hover:text-red-500"
              )}
            >
              <Heart className={cn("w-4 h-4", currentTrack && (currentTrack.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === currentTrack.Id)) && "fill-current")} />
            </button>
          </div>

          {/* Controls */}
          <div className="flex flex-col items-center gap-3 w-1/3">
            <div className="flex items-center gap-4 sm:gap-8">
              <motion.button 
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={() => setIsShuffle(!isShuffle)}
                className={cn(
                  "p-2 transition-colors",
                  isShuffle ? "text-purple-400" : "text-white/20 hover:text-white"
                )}
              >
                <Shuffle className="w-4 h-4" />
              </motion.button>
              
              <motion.button 
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={prevTrack} 
                className="p-2 text-white/60 hover:text-white transition-colors"
              >
                <SkipBack className="w-5 h-5 fill-current" />
              </motion.button>
              
              <motion.button 
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={togglePlay}
                className="w-12 h-12 bg-white rounded-full flex items-center justify-center hover:scale-110 active:scale-95 transition-all shadow-xl shadow-white/10"
              >
                {isPlaying ? <Pause className="text-black fill-black w-5 h-5" /> : <Play className="text-black fill-black w-5 h-5 ml-1" />}
              </motion.button>
              
              <motion.button 
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={nextTrack} 
                className="p-2 text-white/60 hover:text-white transition-colors"
              >
                <SkipForward className="w-5 h-5 fill-current" />
              </motion.button>
              
              <motion.button 
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={() => setIsRepeat(!isRepeat)}
                className={cn(
                  "p-2 transition-colors",
                  isRepeat ? "text-purple-400" : "text-white/20 hover:text-white"
                )}
              >
                <Repeat className="w-4 h-4" />
              </motion.button>
            </div>
            
            <div className="flex items-center gap-4 w-full max-w-md">
              <span className="text-xs font-mono text-white/90 font-bold w-12 text-right">{formatTime(progress)}</span>
              <div 
                className="flex-1 h-1.5 bg-white/20 rounded-full cursor-pointer relative group"
                onClick={(e) => {
                  const rect = e.currentTarget.getBoundingClientRect();
                  const percent = (e.clientX - rect.left) / rect.width;
                  if (audioRef.current) {
                    audioRef.current.currentTime = percent * duration;
                  }
                }}
              >
                {/* Progress Fill */}
                <div 
                  className="absolute inset-y-0 left-0 bg-gradient-to-r from-purple-400 via-blue-400 to-purple-400 bg-[length:200%_100%] animate-gradient-x rounded-full"
                  style={{ width: `${(progress / (duration || 1)) * 100}%` }}
                />
                
                {/* Progress Thumb (Mini) */}
                <motion.div 
                  className="absolute top-1/2 -translate-y-1/2 w-3 h-3 bg-white rounded-full shadow-[0_0_10px_rgba(255,255,255,0.8)] z-20 group-hover:scale-125 transition-transform"
                  style={{ left: `calc(${(progress / (duration || 1)) * 100}% - 6px)` }}
                />
              </div>
              <span className="text-xs font-mono text-white/90 font-bold w-12">{formatTime(duration)}</span>
            </div>
          </div>

          {/* Volume & Extra */}
          <div className="flex items-center justify-end gap-6 w-1/3">
            <motion.button 
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.9 }}
              onClick={getAiInsight}
              disabled={isAiLoading}
              className={cn(
                "p-2 transition-colors relative group",
                aiInsight ? "text-purple-400" : "text-white/20 hover:text-purple-400"
              )}
              title="Aura AI Insight"
            >
              {isAiLoading ? (
                <Loader2 className="w-4 h-4 animate-spin" />
              ) : (
                <Sparkles className="w-4 h-4 group-hover:animate-pulse" />
              )}
              {aiInsight && (
                <span className="absolute -top-1 -right-1 w-2 h-2 bg-purple-500 rounded-full shadow-[0_0_8px_rgba(168,85,247,0.8)]" />
              )}
            </motion.button>

            <div className="flex items-center gap-3 group">
              <Volume2 className="w-4 h-4 text-white/40 group-hover:text-white transition-colors" />
              <PlasmaVolume 
                value={volume}
                onChange={setVolume}
                className="w-24 h-6"
              />
            </div>
            <button 
              className="p-2 glass hover:bg-white/10 rounded-xl transition-all"
              onClick={() => setIsNowPlayingOpen(true)}
            >
              <Maximize2 className="w-4 h-4 text-white/60" />
            </button>
          </div>
        </motion.div>

        {/* Now Playing Fullscreen */}
        <AnimatePresence>
          {isNowPlayingOpen && currentTrack && (
            <motion.div
              initial={{ y: '100%' }}
              animate={{ y: 0 }}
              exit={{ y: '100%' }}
              transition={{ type: 'spring', damping: 25, stiffness: 200 }}
              className="fixed inset-0 z-[60] bg-black/40 backdrop-blur-3xl flex flex-col p-12 overflow-hidden"
            >
              {/* Background Blur */}
              <div className="absolute inset-0 -z-10 overflow-hidden">
                <SynesthesiaBackground data={visualizerData} accentColor={accentColor} zIndex="z-0" enabled={synesthesiaEnabled} />
                <motion.img 
                  initial={{ scale: 1.1 }}
                  animate={{ 
                    scale: [1.1, 1.2, 1.1],
                    rotate: [0, 5, 0]
                  }}
                  transition={{ 
                    duration: 20, 
                    repeat: Infinity, 
                    ease: "linear" 
                  }}
                  src={jellyfin.getAlbumArtUrl(currentTrack.Id)} 
                  alt="" 
                  className="w-full h-full object-cover blur-[120px] opacity-20"
                  referrerPolicy="no-referrer"
                />
                <div className="absolute inset-0 bg-gradient-to-b from-black/20 via-transparent to-black/60" />
              </div>

              <header className="flex items-center justify-between mb-8 relative z-10 px-4">
                <motion.button 
                  initial={{ x: -20, opacity: 0 }}
                  animate={{ x: 0, opacity: 1 }}
                  onClick={() => setIsNowPlayingOpen(false)}
                  className="p-3 glass rounded-2xl hover:bg-white/10 transition-all active:scale-90"
                >
                  <ChevronDown className="w-6 h-6 text-white/70" />
                </motion.button>
                <motion.div 
                  initial={{ y: -20, opacity: 0 }}
                  animate={{ y: 0, opacity: 1 }}
                  className="flex flex-col items-center"
                >
                  <span className="text-[10px] font-bold text-white/30 uppercase tracking-[0.4em] mb-1">Tocando Agora</span>
                  <span className="text-xs font-medium text-white/60 max-w-[200px] truncate">{currentTrack.Album}</span>
                </motion.div>
                
                <div className="flex items-center gap-4">
                  <motion.button 
                    initial={{ x: 20, opacity: 0 }}
                    animate={{ x: 0, opacity: 1 }}
                    onClick={getAiInsight}
                    disabled={isAiLoading}
                    className={cn(
                      "p-3 glass rounded-2xl hover:bg-white/10 transition-all group",
                      aiInsight ? "text-purple-400" : "text-white/70"
                    )}
                  >
                    {isAiLoading ? (
                      <Loader2 className="w-6 h-6 animate-spin" />
                    ) : (
                      <Sparkles className="w-6 h-6 group-hover:animate-pulse" />
                    )}
                  </motion.button>
                  <motion.button 
                    initial={{ x: 20, opacity: 0 }}
                    animate={{ x: 0, opacity: 1 }}
                    className="p-3 glass rounded-2xl hover:bg-white/10 transition-all text-white/70"
                  >
                    <ListMusic className="w-6 h-6" />
                  </motion.button>
                </div>
              </header>

              <div className="flex-1 flex flex-col lg:flex-row items-center justify-center gap-12 lg:gap-24 max-w-6xl mx-auto w-full relative z-10">
                {/* Album Art */}
                <motion.div 
                  initial={{ scale: 0.9, opacity: 0 }}
                  animate={{ scale: 1, opacity: 1 }}
                  whileHover={{ scale: 1.02 }}
                  transition={{ type: "spring", stiffness: 300, damping: 20 }}
                  className={cn(
                    "w-full max-w-[320px] lg:max-w-[450px] aspect-square overflow-hidden shadow-[0_40px_80px_-15px_rgba(0,0,0,0.6)] relative group cursor-pointer",
                    albumArtStyle === 'round' ? "rounded-full animate-spin-slow" : "rounded-[40px] lg:rounded-[60px]"
                  )}
                >
                  <img 
                    src={jellyfin.getAlbumArtUrl(currentTrack.Id)} 
                    alt={currentTrack.Name}
                    className="w-full h-full object-cover"
                    referrerPolicy="no-referrer"
                  />
                  <div className="absolute inset-0 bg-gradient-to-t from-black/40 to-transparent opacity-0 group-hover:opacity-100 transition-opacity duration-500" />
                </motion.div>

                {/* Info & Controls */}
                <div className="flex-1 flex flex-col items-center lg:items-start text-center lg:text-left w-full max-w-xl">
                  <motion.div
                    initial={{ y: 20, opacity: 0 }}
                    animate={{ y: 0, opacity: 1 }}
                    transition={{ delay: 0.1 }}
                    className="mb-10 w-full"
                  >
                    <div className="flex items-center justify-center lg:justify-between gap-4 mb-3">
                      <div className="flex-1 flex flex-col lg:flex-row lg:items-center gap-3">
                        <h2 
                          className="text-4xl lg:text-6xl font-display font-bold tracking-tighter leading-[1.1] line-clamp-2"
                          title={currentTrack.Name}
                        >
                          {currentTrack.Name}
                        </h2>
                      </div>
                      <button 
                        onClick={() => currentTrack && toggleFavorite(currentTrack)}
                        className={cn(
                          "p-3 glass rounded-full hover:bg-white/10 transition-all group",
                          currentTrack && (currentTrack.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === currentTrack.Id)) ? "text-red-500" : "text-white/40 hover:text-red-500"
                        )}
                      >
                        <Heart className={cn("w-6 h-6 group-hover:fill-current transition-colors", currentTrack && (currentTrack.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === currentTrack.Id)) && "fill-current")} />
                      </button>
                    </div>
                    <p className="text-xl lg:text-2xl text-purple-400 font-medium opacity-90">{currentTrack.ArtistItems[0]?.Name}</p>
                    
                    {/* AI Insight Display */}
                    {aiInsight && (
                      <motion.div 
                        initial={{ opacity: 0, y: 10 }}
                        animate={{ opacity: 1, y: 0 }}
                        className="mt-8 p-6 glass rounded-3xl border border-purple-500/30 max-w-md relative group"
                      >
                        <button 
                          onClick={() => setAiInsight(null)}
                          className="absolute top-4 right-4 text-white/20 hover:text-white transition-colors"
                        >
                          <X className="w-4 h-4" />
                        </button>
                        <div className="flex items-center gap-2 mb-3 text-purple-400">
                          <Sparkles className="w-4 h-4" />
                          <span className="text-[10px] font-bold uppercase tracking-[0.2em]">Curiosidade da IA</span>
                        </div>
                        <p className="text-sm leading-relaxed text-white/80 italic">"{aiInsight}"</p>
                      </motion.div>
                    )}
                  </motion.div>

                  {/* Visualizer (Real-time) */}
                  <div className="flex items-end gap-1 h-16 mb-12">
                    {visualizerData.map((value, i) => (
                      <motion.div
                        key={i}
                        animate={{ 
                          height: `${Math.max(10, value * 100)}%`,
                          opacity: 0.2 + value * 0.8
                        }}
                        transition={{ 
                          type: "spring",
                          bounce: 0,
                          duration: 0.1
                        }}
                        className="w-1 bg-white rounded-full"
                      />
                    ))}
                  </div>

                  {/* Controls & Progress */}
                  <motion.div 
                    initial={{ y: 20, opacity: 0 }}
                    animate={{ y: 0, opacity: 1 }}
                    transition={{ delay: 0.2 }}
                    className="w-full space-y-10"
                  >
                    <div className="flex items-center justify-center lg:justify-start gap-8 lg:gap-12">
                      <motion.button 
                        whileHover={{ scale: 1.1 }}
                        whileTap={{ scale: 0.9 }}
                        onClick={() => setIsShuffle(!isShuffle)}
                        className={cn(
                          "p-2 transition-colors",
                          isShuffle ? "text-purple-400" : "text-white/20 hover:text-white"
                        )}
                      >
                        <Shuffle className="w-6 h-6" />
                      </motion.button>
                      
                      <motion.button 
                        whileHover={{ scale: 1.1 }}
                        whileTap={{ scale: 0.9 }}
                        onClick={prevTrack} 
                        className="p-2 text-white/60 hover:text-white transition-all"
                      >
                        <SkipBack className="w-12 h-12 fill-current" />
                      </motion.button>
                      
                      <motion.button 
                        whileHover={{ scale: 1.1 }}
                        whileTap={{ scale: 0.9 }}
                        onClick={togglePlay}
                        className="w-24 h-24 bg-white text-black rounded-full flex items-center justify-center hover:scale-110 active:scale-95 transition-all shadow-2xl shadow-white/10"
                      >
                        {isPlaying ? <Pause className="w-10 h-10 fill-current" /> : <Play className="w-10 h-10 fill-current ml-2" />}
                      </motion.button>
                      
                      <motion.button 
                        whileHover={{ scale: 1.1 }}
                        whileTap={{ scale: 0.9 }}
                        onClick={nextTrack} 
                        className="p-2 text-white/60 hover:text-white transition-all"
                      >
                        <SkipForward className="w-12 h-12 fill-current" />
                      </motion.button>
                      
                      <motion.button 
                        whileHover={{ scale: 1.1 }}
                        whileTap={{ scale: 0.9 }}
                        onClick={() => setIsRepeat(!isRepeat)}
                        className={cn(
                          "p-2 transition-colors",
                          isRepeat ? "text-purple-400" : "text-white/20 hover:text-white"
                        )}
                      >
                        <Repeat className="w-6 h-6" />
                      </motion.button>
                    </div>

                    <div className="space-y-4">
                      <div 
                        className="h-2.5 bg-white/20 rounded-full relative group cursor-pointer"
                        onClick={(e) => {
                          const rect = e.currentTarget.getBoundingClientRect();
                          const percent = (e.clientX - rect.left) / rect.width;
                          if (audioRef.current) {
                            audioRef.current.currentTime = percent * duration;
                          }
                        }}
                      >
                        {/* Progress Fill */}
                        <div 
                          className="absolute inset-y-0 left-0 bg-gradient-to-r from-purple-400 via-blue-400 to-purple-400 bg-[length:200%_100%] animate-gradient-x rounded-full shadow-[0_0_20px_rgba(168,85,247,0.3)]"
                          style={{ width: `${(progress / (duration || 1)) * 100}%` }}
                        />
                        
                        {/* Progress Thumb */}
                        <motion.div 
                          className="absolute top-1/2 -translate-y-1/2 w-5 h-5 bg-white rounded-full shadow-[0_0_20px_rgba(255,255,255,0.8)] z-20 group-hover:scale-110 transition-transform"
                          style={{ left: `calc(${(progress / (duration || 1)) * 100}% - 10px)` }}
                        />
                      </div>
                      
                      <div className="flex justify-between text-base font-mono text-white font-bold tracking-widest">
                        <span className="bg-white/10 px-3 py-1 rounded-lg shadow-lg">{formatTime(progress)}</span>
                        <span className="bg-white/10 px-3 py-1 rounded-lg shadow-lg">{formatTime(duration)}</span>
                      </div>
                    </div>

                    {/* Volume Control (Desktop Only) */}
                    <div className="hidden lg:flex items-center gap-12 pt-8 opacity-60 hover:opacity-100 transition-opacity">
                      <div className="flex flex-col items-center">
                        <span className="text-7xl font-display font-bold text-[#00ff88] drop-shadow-[0_0_20px_rgba(0,255,136,0.6)]">
                          {Math.round(volume * 100)}
                        </span>
                        <span className="text-[10px] font-bold text-white/30 uppercase tracking-[0.4em] mt-2">Volume %</span>
                        <span className="text-xs font-bold text-[#00ff88] uppercase tracking-widest mt-1 opacity-80">
                          {volume === 0 ? 'Mudo' : volume < 0.3 ? 'Baixo' : volume < 0.7 ? 'Ideal' : 'Alto'}
                        </span>
                      </div>
                      <div className="flex-1 flex items-center gap-6">
                        <Volume2 className="w-8 h-8 text-white" />
                        <PlasmaVolume 
                          value={volume}
                          onChange={setVolume}
                          className="flex-1 h-10"
                        />
                      </div>
                    </div>
                  </motion.div>
                </div>
              </div>
            </motion.div>
          )}
        </AnimatePresence>

        {/* SVG Filter for Plasma Effect */}
        <svg style={{ position: 'absolute', width: 0, height: 0 }}>
          <defs>
            <filter id="turbulent-displace" colorInterpolationFilters="sRGB" x="-20%" y="-20%" width="140%" height="140%">
              <feTurbulence type="turbulence" baseFrequency="0.02" numOctaves="10" result="noise1" seed="1" />
              <feOffset in="noise1" dx="0" dy="0" result="offsetNoise1">
                <animate attributeName="dy" values="700; 0" dur="6s" repeatCount="indefinite" calcMode="linear" />
              </feOffset>
              <feTurbulence type="turbulence" baseFrequency="0.02" numOctaves="10" result="noise2" seed="1" />
              <feOffset in="noise2" dx="0" dy="0" result="offsetNoise2">
                <animate attributeName="dy" values="0; -700" dur="6s" repeatCount="indefinite" calcMode="linear" />
              </feOffset>
              <feTurbulence type="turbulence" baseFrequency="0.02" numOctaves="10" result="noise3" seed="2" />
              <feOffset in="noise3" dx="0" dy="0" result="offsetNoise3">
                <animate attributeName="dx" values="490; 0" dur="6s" repeatCount="indefinite" calcMode="linear" />
              </feOffset>
              <feTurbulence type="turbulence" baseFrequency="0.02" numOctaves="10" result="noise4" seed="2" />
              <feOffset in="noise4" dx="0" dy="0" result="offsetNoise4">
                <animate attributeName="dx" values="0; -490" dur="6s" repeatCount="indefinite" calcMode="linear" />
              </feOffset>
              <feComposite in="offsetNoise1" in2="offsetNoise2" result="part1" />
              <feComposite in="offsetNoise3" in2="offsetNoise4" result="part2" />
              <feBlend in="part1" in2="part2" mode="color-dodge" result="combinedNoise" />
              <feDisplacementMap in="SourceGraphic" in2="combinedNoise" scale="30" xChannelSelector="R" yChannelSelector="B" />
            </filter>
          </defs>
        </svg>
      </main>
    </div>
  );
}

const NavItem = ({ icon, label, active = false, onClick }: { icon: React.ReactNode, label: string, active?: boolean, onClick?: () => void }) => (
  <button 
    onClick={onClick}
    className={cn(
      "w-full flex items-center gap-3 px-4 py-3 rounded-xl transition-all duration-500 group relative overflow-hidden",
      active 
        ? "bg-white/10 text-white shadow-[0_0_20px_rgba(255,255,255,0.05)]" 
        : "text-white/40 hover:text-white hover:bg-white/5"
    )}
  >
    {active && (
      <motion.div 
        layoutId="nav-active"
        className="absolute left-0 w-1 h-5 bg-purple-500 rounded-r-full"
      />
    )}
    <div className={cn("transition-transform duration-500", active ? "scale-110" : "group-hover:scale-110")}>
      {icon}
    </div>
    <span className="text-sm font-medium">{label}</span>
  </button>
);

const TrackCard: React.FC<{ 
  track: Track, 
  index: number, 
  onPlay: (t: Track) => void, 
  albumArtStyle: 'square' | 'round',
  onToggleFavorite?: (t: Track) => void,
  isFavorite?: boolean
}> = ({ track, index, onPlay, albumArtStyle, onToggleFavorite, isFavorite }) => (
  <motion.div
    initial={{ opacity: 0, y: 20 }}
    animate={{ opacity: 1, y: 0 }}
    transition={{ delay: Math.min(index * 0.05, 0.5) }}
    className="group relative"
  >
    <div 
      className={cn(
        "aspect-square overflow-hidden glass relative cursor-pointer group-hover:scale-[1.05] group-hover:-translate-y-2 transition-all duration-700 shadow-2xl",
        albumArtStyle === 'round' ? "rounded-full animate-spin-slow" : "rounded-[32px]"
      )}
    >
      <img 
        src={jellyfin.getAlbumArtUrl(track.Id)} 
        alt={track.Name}
        onClick={() => onPlay(track)}
        className="w-full h-full object-cover opacity-80 group-hover:opacity-100 transition-all duration-700 group-hover:scale-110"
        referrerPolicy="no-referrer"
      />
      
      <div className="absolute top-4 right-4 z-10 opacity-0 group-hover:opacity-100 transition-opacity duration-300">
        <button 
          onClick={(e) => {
            e.stopPropagation();
            onToggleFavorite?.(track);
          }}
          className={cn(
            "p-3 rounded-full glass hover:scale-110 transition-all active:scale-90",
            isFavorite ? "text-red-500" : "text-white/40 hover:text-white"
          )}
        >
          <Heart className={cn("w-5 h-5", isFavorite && "fill-current")} />
        </button>
      </div>

      <div 
        onClick={() => onPlay(track)}
        className="absolute inset-0 bg-gradient-to-t from-black/80 via-transparent to-transparent opacity-0 group-hover:opacity-100 transition-opacity duration-500 flex items-center justify-center"
      >
        <motion.div 
          whileHover={{ scale: 1.1 }}
          whileTap={{ scale: 0.9 }}
          className="w-16 h-16 bg-white rounded-full flex items-center justify-center shadow-2xl"
        >
          <Play className="text-black fill-black w-7 h-7 ml-1" />
        </motion.div>
      </div>
    </div>
    <div className="mt-5 px-3">
      <h3 className="font-bold truncate text-sm tracking-tight group-hover:text-purple-400 transition-colors">{track.Name}</h3>
      <p className="text-[11px] font-medium text-white/30 truncate mt-1.5 uppercase tracking-wider">{track.ArtistItems[0]?.Name || 'Artista Desconhecido'}</p>
    </div>
  </motion.div>
);
