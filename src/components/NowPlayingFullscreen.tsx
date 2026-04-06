import React from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { 
  ChevronDown, Loader2, Sparkles, ListMusic, Heart, 
  Shuffle, SkipBack, Pause, Play, SkipForward, Repeat 
} from 'lucide-react';
import { Track, jellyfin } from '../services/jellyfin';
import { cn, formatTime } from '../lib/utils';
import { SynesthesiaBackground } from './SynesthesiaBackground';
import { AIInsightCard } from './AIInsightCard';

interface NowPlayingFullscreenProps {
  isOpen: boolean;
  onClose: () => void;
  currentTrack: Track | null;
  isPlaying: boolean;
  togglePlay: () => void;
  nextTrack: () => void;
  prevTrack: () => void;
  progress: number;
  duration: number;
  onSeek: (percent: number) => void;
  volume: number;
  onVolumeChange: (val: number) => void;
  isShuffle: boolean;
  setIsShuffle: (val: boolean) => void;
  isRepeat: boolean;
  setIsRepeat: (val: boolean) => void;
  visualizerData: number[];
  accentColor: string;
  synesthesiaEnabled: boolean;
  albumArtStyle: 'square' | 'round';
  favoriteTracks: Track[];
  toggleFavorite: (track: Track) => void;
  aiInsight: string | null;
  setAiInsight: (val: string | null) => void;
  getAiInsight: () => void;
  isAiLoading: boolean;
}

export const NowPlayingFullscreen: React.FC<NowPlayingFullscreenProps> = ({
  isOpen,
  onClose,
  currentTrack,
  isPlaying,
  togglePlay,
  nextTrack,
  prevTrack,
  progress,
  duration,
  onSeek,
  isShuffle,
  setIsShuffle,
  isRepeat,
  setIsRepeat,
  visualizerData,
  accentColor,
  synesthesiaEnabled,
  albumArtStyle,
  favoriteTracks,
  toggleFavorite,
  aiInsight,
  setAiInsight,
  getAiInsight,
  isAiLoading
}) => {
  if (!currentTrack) return null;

  const isFavorite = currentTrack.UserData?.IsFavorite || favoriteTracks.some(t => t.Id === currentTrack.Id);

  return (
    <AnimatePresence>
      {isOpen && (
        <motion.div
          initial={{ y: '100%' }}
          animate={{ y: 0 }}
          exit={{ y: '100%' }}
          transition={{ type: 'spring', damping: 25, stiffness: 200 }}
          className="fixed inset-0 z-[60] bg-black/60 backdrop-blur-2xl flex flex-col p-12 overflow-hidden"
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
              onClick={onClose}
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
                    onClick={() => toggleFavorite(currentTrack)}
                    className={cn(
                      "p-3 glass rounded-full hover:bg-white/10 transition-all group",
                      isFavorite ? "text-red-500" : "text-white/40 hover:text-red-500"
                    )}
                  >
                    <Heart className={cn("w-6 h-6 group-hover:fill-current transition-colors", isFavorite && "fill-current")} />
                  </button>
                </div>
                <p className="text-xl lg:text-2xl text-purple-400 font-medium opacity-90">{currentTrack.ArtistItems[0]?.Name}</p>
                
                <AIInsightCard insight={aiInsight} onClose={() => setAiInsight(null)} />
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
                      onSeek(percent);
                    }}
                  >
                    {/* Progress Fill */}
                    <div 
                      className="absolute inset-y-0 left-0 bg-gradient-to-r from-purple-400 via-blue-400 to-purple-400 bg-[length:200%_100%] animate-gradient-x rounded-full"
                      style={{ width: `${(progress / (duration || 1)) * 100}%` }}
                    />
                    
                    {/* Progress Thumb */}
                    <motion.div 
                      className="absolute top-1/2 -translate-y-1/2 w-5 h-5 bg-white rounded-full shadow-[0_0_20px_rgba(255,255,255,0.8)] z-20 scale-0 group-hover:scale-100 transition-transform"
                      style={{ left: `calc(${(progress / (duration || 1)) * 100}% - 10px)` }}
                    />
                  </div>
                  <div className="flex justify-between text-xs font-mono font-bold text-white/40 tracking-widest">
                    <span>{formatTime(progress)}</span>
                    <span>{formatTime(duration)}</span>
                  </div>
                </div>
              </motion.div>
            </div>
          </div>
        </motion.div>
      )}
    </AnimatePresence>
  );
};
