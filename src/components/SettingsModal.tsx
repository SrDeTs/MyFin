import React from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { 
  X, Settings as SettingsIcon, User, Shield, 
  Zap, Volume2, PlayCircle, Clock, Shuffle, 
  Disc, Music, Activity, LogOut, Languages,
  Palette, Sparkles,
  Image as ImageIcon, Type, Bell, MessageSquare,
  LayoutGrid
} from 'lucide-react';
import { cn } from '../lib/utils';
import { ThemeSwitch } from './ui/ThemeSwitch';

interface SettingsModalProps {
  isOpen: boolean;
  onClose: () => void;
  onLogout: () => void;
  user: any;
  theme: 'dark' | 'light';
  setTheme: (theme: 'dark' | 'light') => void;
  synesthesiaEnabled: boolean;
  setSynesthesiaEnabled: (val: boolean) => void;
  albumArtStyle: 'square' | 'round';
  setAlbumArtStyle: (val: 'square' | 'round') => void;
  equalizerPreset: string;
  setEqualizerPreset: (val: string) => void;
  crossfadeTime: number;
  setCrossfadeTime: (val: number) => void;
  sleepTimer: number | null;
  setSleepTimer: (val: number | null) => void;
  autoPlay: boolean;
  setAutoPlay: (val: boolean) => void;
  volumeNormalization: boolean;
  setVolumeNormalization: (val: boolean) => void;
  audioQuality: 'Auto' | '320kbps' | '128kbps';
  setAudioQuality: (val: 'Auto' | '320kbps' | '128kbps') => void;
  audioFilter: 'none' | 'vintage' | 'vinyl' | 'lofi';
  setAudioFilter: (val: 'none' | 'vintage' | 'vinyl' | 'lofi') => void;
  accentColor: 'purple' | 'blue' | 'green' | 'pink' | 'orange';
  setAccentColor: (val: 'purple' | 'blue' | 'green' | 'pink' | 'orange') => void;
  bgAnimationEnabled: boolean;
  setBgAnimationEnabled: (val: boolean) => void;
  bgAnimationSpeed: 'slow' | 'normal' | 'fast';
  setBgAnimationSpeed: (val: 'slow' | 'normal' | 'fast') => void;
  fontFamily: 'sans' | 'display' | 'serif';
  setFontFamily: (val: 'sans' | 'display' | 'serif') => void;
  language: 'pt' | 'en' | 'es';
  setLanguage: (val: 'pt' | 'en' | 'es') => void;
  notificationsEnabled: boolean;
  setNotificationsEnabled: (val: boolean) => void;
  lastfmEnabled: boolean;
  setLastfmEnabled: (val: boolean) => void;
  immersiveMode: boolean;
  setImmersiveMode: (val: boolean) => void;
}

export const SettingsModal: React.FC<SettingsModalProps> = ({
  isOpen,
  onClose,
  onLogout,
  user,
  theme,
  setTheme,
  synesthesiaEnabled,
  setSynesthesiaEnabled,
  albumArtStyle,
  setAlbumArtStyle,
  equalizerPreset,
  setEqualizerPreset,
  crossfadeTime,
  setCrossfadeTime,
  sleepTimer,
  setSleepTimer,
  autoPlay,
  setAutoPlay,
  volumeNormalization,
  setVolumeNormalization,
  audioQuality,
  setAudioQuality,
  audioFilter,
  setAudioFilter,
  accentColor,
  setAccentColor,
  bgAnimationEnabled,
  setBgAnimationEnabled,
  bgAnimationSpeed,
  setBgAnimationSpeed,
  fontFamily,
  setFontFamily,
  language,
  setLanguage,
  notificationsEnabled,
  setNotificationsEnabled,
  lastfmEnabled,
  setLastfmEnabled,
  immersiveMode,
  setImmersiveMode
}) => {
  const [settingsTab, setSettingsTab] = React.useState<'audio' | 'account' | 'advanced' | 'soundlab' | 'interface'>('interface');

  if (!isOpen) return null;

  return (
    <AnimatePresence>
      <motion.div 
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        className="fixed inset-0 z-[100] flex items-center justify-center p-4 bg-black/80 backdrop-blur-xl"
      >
        <motion.div 
          initial={{ scale: 0.9, y: 20 }}
          animate={{ scale: 1, y: 0 }}
          exit={{ scale: 0.9, y: 20 }}
          className="w-full max-w-4xl glass rounded-[40px] overflow-hidden flex flex-col lg:flex-row h-[85vh] shadow-2xl border border-white/10"
        >
          {/* Sidebar */}
          <div className="w-full lg:w-72 bg-white/5 p-8 flex flex-col border-r border-white/5">
            <div className="flex items-center gap-4 mb-12">
              <div className="w-12 h-12 rounded-2xl bg-primary/20 flex items-center justify-center text-primary">
                <SettingsIcon className="w-6 h-6" />
              </div>
              <h2 className="text-2xl font-display font-bold tracking-tight">Configurações</h2>
            </div>

            <div className="space-y-2 flex-1">
              {[
                { id: 'interface', label: 'Interface', icon: <Palette className="w-4 h-4" /> },
                { id: 'audio', label: 'Áudio', icon: <Volume2 className="w-4 h-4" /> },
                { id: 'soundlab', label: 'Sound Lab', icon: <Zap className="w-4 h-4" /> },
                { id: 'account', label: 'Conta', icon: <User className="w-4 h-4" /> },
                { id: 'advanced', label: 'Avançado', icon: <Shield className="w-4 h-4" /> },
              ].map((tab) => (
                <button
                  key={tab.id}
                  onClick={() => setSettingsTab(tab.id as any)}
                  className={cn(
                    "w-full flex items-center gap-4 px-6 py-4 rounded-2xl transition-all font-medium",
                    settingsTab === tab.id 
                      ? "bg-primary text-white shadow-lg shadow-primary/20" 
                      : "text-white/40 hover:text-white hover:bg-white/5"
                  )}
                >
                  {tab.icon}
                  {tab.label}
                </button>
              ))}
            </div>

            <button 
              onClick={onLogout}
              className="mt-auto flex items-center gap-4 px-6 py-4 rounded-2xl text-red-400 hover:bg-red-500/10 transition-all font-medium"
            >
              <LogOut className="w-4 h-4" />
              Sair da Conta
            </button>
          </div>

          {/* Content */}
          <div className="flex-1 p-8 lg:p-12 overflow-y-auto relative">
            <button 
              onClick={onClose}
              className="absolute top-8 right-8 p-3 glass rounded-2xl hover:bg-white/10 transition-all z-10"
            >
              <X className="w-5 h-5" />
            </button>

            <div className="min-h-[400px] mb-20">
              <AnimatePresence mode="wait">
                {settingsTab === 'interface' && (
                  <motion.div 
                    key="interface"
                    initial={{ opacity: 0, x: 20 }}
                    animate={{ opacity: 1, x: 0 }}
                    exit={{ opacity: 0, x: -20 }}
                    className="space-y-8"
                  >
                    <div className="space-y-6">
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
                    initial={{ opacity: 0, x: 20 }}
                    animate={{ opacity: 1, x: 0 }}
                    exit={{ opacity: 0, x: -20 }}
                    className="space-y-8"
                  >
                    <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
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
                        <div className="grid grid-cols-2 gap-2">
                          {['none', 'Bass Boost', 'Treble', 'Vocal'].map((eq) => (
                            <button 
                              key={eq}
                              onClick={() => setEqualizerPreset(eq)}
                              className={cn(
                                "py-2 rounded-xl text-xs font-bold transition-all border",
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
                  initial={{ opacity: 0, x: 20 }}
                  animate={{ opacity: 1, x: 0 }}
                  exit={{ opacity: 0, x: -20 }}
                  className="space-y-8"
                >
                  <div className="space-y-6">
                    <h3 className="text-xs font-bold text-white/30 uppercase tracking-widest">Laboratório de Som</h3>
                    
                    {/* Synesthesia Toggle */}
                    <button 
                      onClick={() => setSynesthesiaEnabled(!synesthesiaEnabled)}
                      className="w-full flex items-center justify-between p-5 glass rounded-2xl hover:bg-white/5 transition-all group"
                    >
                      <div className="flex items-center gap-4">
                        <div className={cn(
                          "w-10 h-10 rounded-xl flex items-center justify-center transition-colors",
                          synesthesiaEnabled ? "bg-primary/20 text-primary" : "bg-white/5 text-white/20"
                        )}>
                          <Zap className="w-5 h-5" />
                        </div>
                        <div className="text-left">
                          <span className="font-bold block">Sinestesia Visual</span>
                          <span className="text-xs text-white/40">Fundo reativo ao ritmo da música</span>
                        </div>
                      </div>
                      <div className={cn(
                        "w-12 h-6 rounded-full relative transition-colors duration-300",
                        synesthesiaEnabled ? "bg-primary" : "bg-white/10"
                      )}>
                        <motion.div 
                          animate={{ x: synesthesiaEnabled ? 24 : 4 }}
                          className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                        />
                      </div>
                    </button>

                    <div className="space-y-4">
                      <div className="flex items-center gap-2 text-white/40 mb-2">
                        <Activity className="w-4 h-4" />
                        <span className="text-xs font-bold uppercase tracking-widest">Filtros de Era</span>
                      </div>
                      <div className="grid grid-cols-2 gap-4">
                        {[
                          { id: 'none', name: 'Original', desc: 'Som puro', icon: <Music className="w-5 h-5" /> },
                          { id: 'vintage', name: 'Rádio 1940', desc: 'Vintage', icon: <Activity className="w-5 h-5" /> },
                          { id: 'vinyl', name: 'Vinil Quente', desc: 'Caloroso', icon: <Disc className="w-5 h-5" /> },
                          { id: 'lofi', name: 'Lo-Fi', desc: 'Abafado', icon: <Clock className="w-5 h-5" /> },
                        ].map((filter) => (
                          <button
                            key={filter.id}
                            onClick={() => setAudioFilter(filter.id as any)}
                            className={cn(
                              "p-4 rounded-2xl border-2 text-left transition-all",
                              audioFilter === filter.id 
                                ? "border-primary bg-primary/10" 
                                : "border-white/5 bg-white/5 hover:border-white/10"
                            )}
                          >
                            <div className="font-bold text-sm mb-1">{filter.name}</div>
                            <div className="text-[10px] text-white/40">{filter.desc}</div>
                          </button>
                        ))}
                      </div>
                    </div>
                  </div>
                </motion.div>
              )}

              {settingsTab === 'account' && (
                <motion.div 
                  key="account"
                  initial={{ opacity: 0, x: 20 }}
                  animate={{ opacity: 1, x: 0 }}
                  exit={{ opacity: 0, x: -20 }}
                  className="space-y-8"
                >
                  <div className="p-8 glass rounded-[32px] flex items-center gap-8">
                    <div className="w-24 h-24 rounded-3xl bg-primary/20 flex items-center justify-center text-primary text-4xl font-display font-bold">
                      {user?.Name?.charAt(0).toUpperCase()}
                    </div>
                    <div>
                      <h3 className="text-2xl font-bold mb-1">{user?.Name}</h3>
                      <p className="text-white/40 text-sm mb-4">ID: {user?.Id}</p>
                      <div className="flex gap-2">
                        <span className="px-3 py-1 bg-primary/20 text-primary text-[10px] font-bold uppercase tracking-widest rounded-full">Pro Member</span>
                        <span className="px-3 py-1 bg-white/5 text-white/40 text-[10px] font-bold uppercase tracking-widest rounded-full">Aura Early Access</span>
                      </div>
                    </div>
                  </div>

                  <div className="grid grid-cols-2 gap-4">
                    <div className="p-6 glass rounded-2xl">
                      <div className="flex items-center gap-3 mb-4 text-white/40">
                        <Languages className="w-4 h-4" />
                        <span className="text-xs font-bold uppercase tracking-widest">Idioma</span>
                      </div>
                      <p className="font-bold">Português (Brasil)</p>
                    </div>
                    <div className="p-6 glass rounded-2xl">
                      <div className="flex items-center gap-3 mb-4 text-white/40">
                        <Clock className="w-4 h-4" />
                        <span className="text-xs font-bold uppercase tracking-widest">Membro desde</span>
                      </div>
                      <p className="font-bold">Abril 2024</p>
                    </div>
                  </div>
                </motion.div>
              )}

              {settingsTab === 'advanced' && (
                <motion.div 
                  key="advanced"
                  initial={{ opacity: 0, x: 20 }}
                  animate={{ opacity: 1, x: 0 }}
                  exit={{ opacity: 0, x: -20 }}
                  className="space-y-8"
                >
                  <div className="space-y-4">
                    <div className="flex items-center justify-between p-6 glass rounded-2xl">
                      <div>
                        <p className="font-bold mb-1">Tema do Sistema</p>
                        <p className="text-xs text-white/40">Alternar entre modo claro e escuro</p>
                      </div>
                      <ThemeSwitch isDark={theme === 'dark'} onToggle={() => setTheme(theme === 'dark' ? 'light' : 'dark')} />
                    </div>

                    <div className="flex items-center justify-between p-6 glass rounded-2xl">
                      <div>
                        <p className="font-bold mb-1">Sinestesia Visual</p>
                        <p className="text-xs text-white/40">Fundo reativo à energia da música</p>
                      </div>
                      <button 
                        onClick={() => setSynesthesiaEnabled(!synesthesiaEnabled)}
                        className={cn(
                          "w-12 h-6 rounded-full relative transition-colors duration-300",
                          synesthesiaEnabled ? "bg-primary" : "bg-white/10"
                        )}
                      >
                        <motion.div 
                          animate={{ x: synesthesiaEnabled ? 24 : 4 }}
                          className="absolute top-1 w-4 h-4 bg-white rounded-full shadow-lg"
                        />
                      </button>
                    </div>

                    <div className="p-6 glass rounded-2xl">
                      <p className="font-bold mb-4">Estilo da Capa</p>
                      <div className="flex gap-4">
                        <button 
                          onClick={() => setAlbumArtStyle('square')}
                          className={cn(
                            "flex-1 py-3 rounded-xl text-xs font-bold transition-all border",
                            albumArtStyle === 'square' ? "bg-primary border-primary text-white" : "bg-white/5 border-white/5 text-white/40"
                          )}
                        >
                          Quadrado Moderno
                        </button>
                        <button 
                          onClick={() => setAlbumArtStyle('round')}
                          className={cn(
                            "flex-1 py-3 rounded-xl text-xs font-bold transition-all border",
                            albumArtStyle === 'round' ? "bg-primary border-primary text-white" : "bg-white/5 border-white/5 text-white/40"
                          )}
                        >
                          Vinil Clássico
                        </button>
                      </div>
                    </div>
                  </div>
                </motion.div>
              )}
            </AnimatePresence>
          </div>
        </div>
      </motion.div>
    </motion.div>
  </AnimatePresence>
);
};

