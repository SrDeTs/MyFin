import React, { useState, useEffect, useRef } from 'react';
import { AnimatePresence } from 'motion/react';
import { jellyfin, Track, Playlist } from './services/jellyfin';
import { cn } from './lib/utils';
import { SynesthesiaBackground } from './components/SynesthesiaBackground';
import { LoginScreen } from './components/LoginScreen';
import { NowPlayingFullscreen } from './components/NowPlayingFullscreen';
import { PlayerBar } from './components/PlayerBar';
import { SettingsModal } from './components/SettingsModal';
import { Sidebar } from './components/Sidebar';
import { Header } from './components/Header';
import { ExploreView } from './components/views/ExploreView';
import { AlbumsView } from './components/views/AlbumsView';
import { ArtistsView } from './components/views/ArtistsView';
import { FavoritesView } from './components/views/FavoritesView';
import { PlaylistsView } from './components/views/PlaylistsView';

// --- Main App ---

export default function App() {
  const [isLoggedIn, setIsLoggedIn] = useState(jellyfin.isLoggedIn());
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
  const user = jellyfin.getConfig();

  const handleLogout = () => {
    jellyfin.logout();
    setIsLoggedIn(false);
    setIsSettingsOpen(false);
  };
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
  
  // Auto-save settings
  useEffect(() => {
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
    localStorage.setItem('aura_synesthesia_enabled', synesthesiaEnabled.toString());
  }, [
    immersiveMode, volumeNormalization, bgAnimationEnabled, bgAnimationSpeed,
    albumArtStyle, equalizerPreset, crossfadeTime, autoPlay, notificationsEnabled,
    lastfmEnabled, language, synesthesiaEnabled
  ]);

  const audioRef = useRef<HTMLAudioElement | null>(null);
  const sleepTimerRef = useRef<NodeJS.Timeout | null>(null);
  const analyserRef = useRef<AnalyserNode | null>(null);
  const dataArrayRef = useRef<Uint8Array | null>(null);
  const animationFrameRef = useRef<number | null>(null);
  const audioContextRef = useRef<AudioContext | null>(null);
  const filterNodeRef = useRef<BiquadFilterNode | null>(null);
  const filterNode2Ref = useRef<BiquadFilterNode | null>(null);

  useEffect(() => {
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
  }, [isLoggedIn]);

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
  }, [audioFilter]);

  const getAiInsight = async () => {
    if (!currentTrack) return;
    setIsAiLoading(true);
    setAiInsight(null);
    try {
      const { GoogleGenAI } = await import("@google/genai");
      const ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });
      const response = await ai.models.generateContent({
        model: "gemini-3-flash-preview",
        contents: `Conte uma curiosidade curta, fascinante e pouco conhecida sobre a música "${currentTrack.Name}" do artista "${currentTrack.ArtistItems[0]?.Name}". Seja direto e use um tom de entusiasta de música.`,
      });
      setAiInsight(response.text || "Não consegui encontrar curiosidades sobre esta faixa.");
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

  const handlePlaylistClick = async (playlist: Playlist) => {
    const items = await jellyfin.getPlaylistItems(playlist.Id);
    setTracks(items);
    setActiveView('explore');
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

  const albums = Array.from(new Set(tracks.map(t => t.Album))).filter(Boolean) as string[];
  const artists = Array.from(new Set(tracks.flatMap(t => t.ArtistItems.map(a => a.Name)))).filter(Boolean) as string[];

  if (!isLoggedIn) {
    return <LoginScreen onLogin={() => setIsLoggedIn(true)} isDark={isDark} onToggleTheme={() => setIsDark(!isDark)} bgAnimationEnabled={bgAnimationEnabled} bgAnimationSpeed={bgAnimationSpeed} />;
  }

  return (
    <div className="flex h-screen overflow-hidden bg-transparent font-sans relative">
      <SynesthesiaBackground data={visualizerData} accentColor={accentColor} enabled={synesthesiaEnabled && !isNowPlayingOpen} />
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
        crossOrigin="anonymous"
      />

      {/* Sidebar */}
      <Sidebar 
        immersiveMode={immersiveMode}
        isSettingsOpen={isSettingsOpen}
        activeView={activeView}
        setActiveView={setActiveView}
        setIsSettingsOpen={setIsSettingsOpen}
      />

      {/* Main Content */}
      <main className="flex-1 flex flex-col relative overflow-hidden">
        {/* Header */}
        <Header 
          searchQuery={searchQuery}
          setSearchQuery={setSearchQuery}
          isDark={isDark}
          setIsDark={setIsDark}
          loadTracks={loadTracks}
          isRefreshing={isRefreshing}
          user={user}
        />

        {/* Scrollable Area */}
        <div className="flex-1 overflow-y-auto px-8 pb-40 custom-scrollbar">
          <AnimatePresence mode="wait">
            {activeView === 'explore' && (
              <ExploreView 
                filteredTracks={filteredTracks}
                playTrack={playTrack}
                albumArtStyle={albumArtStyle}
                toggleFavorite={toggleFavorite}
                favoriteTracks={favoriteTracks}
              />
            )}

            {activeView === 'albums' && (
              <AlbumsView albums={albums} />
            )}

            {activeView === 'artists' && (
              <ArtistsView artists={artists} />
            )}

            {activeView === 'favorites' && (
              <FavoritesView 
                favoriteTracks={favoriteTracks}
                playTrack={playTrack}
                albumArtStyle={albumArtStyle}
                toggleFavorite={toggleFavorite}
              />
            )}

            {activeView === 'playlists' && (
              <PlaylistsView 
                playlists={playlists}
                onPlaylistClick={handlePlaylistClick}
              />
            )}
          </AnimatePresence>
        </div>

        {/* Settings Modal */}
        <SettingsModal 
          isOpen={isSettingsOpen}
          onClose={() => setIsSettingsOpen(false)}
          onLogout={handleLogout}
          user={user}
          theme={isDark ? 'dark' : 'light'}
          setTheme={(t) => setIsDark(t === 'dark')}
          synesthesiaEnabled={synesthesiaEnabled}
          setSynesthesiaEnabled={setSynesthesiaEnabled}
          albumArtStyle={albumArtStyle}
          setAlbumArtStyle={setAlbumArtStyle}
          equalizerPreset={equalizerPreset}
          setEqualizerPreset={setEqualizerPreset}
          crossfadeTime={crossfadeTime}
          setCrossfadeTime={setCrossfadeTime}
          sleepTimer={sleepTimer}
          setSleepTimer={setSleepTimer}
          autoPlay={autoPlay}
          setAutoPlay={setAutoPlay}
          volumeNormalization={volumeNormalization}
          setVolumeNormalization={setVolumeNormalization}
          audioQuality={audioQuality}
          setAudioQuality={setAudioQuality}
          audioFilter={audioFilter}
          setAudioFilter={setAudioFilter}
          accentColor={accentColor}
          setAccentColor={setAccentColor}
          bgAnimationEnabled={bgAnimationEnabled}
          setBgAnimationEnabled={setBgAnimationEnabled}
          bgAnimationSpeed={bgAnimationSpeed}
          setBgAnimationSpeed={setBgAnimationSpeed}
          fontFamily={fontFamily}
          setFontFamily={setFontFamily}
          language={language}
          setLanguage={setLanguage}
          notificationsEnabled={notificationsEnabled}
          setNotificationsEnabled={setNotificationsEnabled}
          lastfmEnabled={lastfmEnabled}
          setLastfmEnabled={setLastfmEnabled}
          immersiveMode={immersiveMode}
          setImmersiveMode={setImmersiveMode}
        />

        {/* Player Bar */}
        <PlayerBar 
          currentTrack={currentTrack}
          isPlaying={isPlaying}
          togglePlay={togglePlay}
          nextTrack={nextTrack}
          prevTrack={prevTrack}
          progress={progress}
          duration={duration}
          onSeek={(percent) => {
            if (audioRef.current) {
              audioRef.current.currentTime = percent * duration;
            }
          }}
          volume={volume}
          onVolumeChange={setVolume}
          aiInsight={aiInsight}
          getAiInsight={getAiInsight}
          isAiLoading={isAiLoading}
          onOpenFullscreen={() => setIsNowPlayingOpen(true)}
        />

        {/* Now Playing Fullscreen */}
        <NowPlayingFullscreen 
          isOpen={isNowPlayingOpen}
          onClose={() => setIsNowPlayingOpen(false)}
          currentTrack={currentTrack}
          isPlaying={isPlaying}
          togglePlay={togglePlay}
          nextTrack={nextTrack}
          prevTrack={prevTrack}
          progress={progress}
          duration={duration}
          onSeek={(percent) => {
            if (audioRef.current) {
              audioRef.current.currentTime = percent * duration;
            }
          }}
          volume={volume}
          onVolumeChange={setVolume}
          isShuffle={isShuffle}
          setIsShuffle={setIsShuffle}
          isRepeat={isRepeat}
          setIsRepeat={setIsRepeat}
          visualizerData={visualizerData}
          accentColor={accentColor}
          synesthesiaEnabled={synesthesiaEnabled}
          albumArtStyle={albumArtStyle}
          favoriteTracks={favoriteTracks}
          toggleFavorite={toggleFavorite}
          aiInsight={aiInsight}
          setAiInsight={setAiInsight}
          getAiInsight={getAiInsight}
          isAiLoading={isAiLoading}
        />

      </main>
    </div>
  );
}

// --- Helpers ---
