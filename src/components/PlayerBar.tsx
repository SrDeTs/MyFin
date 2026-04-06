import React from 'react';
import { motion } from 'motion/react';
import { 
  SkipBack, Pause, Play, SkipForward, 
  Loader2, Sparkles, Volume2, Maximize2 
} from 'lucide-react';
import { Track, jellyfin } from '../services/jellyfin';
import { cn, formatTime } from '../lib/utils';
import { PlasmaVolume } from './ui/PlasmaVolume';

interface PlayerBarProps {
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
  aiInsight: string | null;
  getAiInsight: () => void;
  isAiLoading: boolean;
  onOpenFullscreen: () => void;
}

export const PlayerBar: React.FC<PlayerBarProps> = ({
  currentTrack,
  isPlaying,
  togglePlay,
  nextTrack,
  prevTrack,
  progress,
  duration,
  onSeek,
  volume,
  onVolumeChange,
  aiInsight,
  getAiInsight,
  isAiLoading,
  onOpenFullscreen
}) => {
  if (!currentTrack) return null;

  return (
    <motion.div 
      initial={{ y: 100 }}
      animate={{ y: 0 }}
      className="fixed bottom-8 left-8 right-8 h-24 glass rounded-[32px] flex items-center px-8 gap-8 z-50 border border-white/10 shadow-[0_20px_50px_rgba(0,0,0,0.5)]"
    >
      {/* Track Info */}
      <div className="flex items-center gap-4 w-1/3">
        <div 
          className="w-14 h-14 rounded-2xl overflow-hidden shadow-lg cursor-pointer group relative"
          onClick={onOpenFullscreen}
        >
          <img 
            src={jellyfin.getAlbumArtUrl(currentTrack.Id)} 
            alt={currentTrack.Name}
            className="w-full h-full object-cover group-hover:scale-110 transition-transform duration-500"
            referrerPolicy="no-referrer"
          />
          <div className="absolute inset-0 bg-black/20 opacity-0 group-hover:opacity-100 transition-opacity flex items-center justify-center">
            <Maximize2 className="w-4 h-4 text-white" />
          </div>
        </div>
        <div className="flex flex-col min-w-0">
          <h4 className="font-bold truncate text-sm tracking-tight">{currentTrack.Name}</h4>
          <p className="text-[11px] text-white/40 truncate font-medium uppercase tracking-wider mt-0.5">{currentTrack.ArtistItems[0]?.Name}</p>
        </div>
      </div>

      {/* Controls & Progress */}
      <div className="flex-1 flex flex-col items-center gap-3">
        <div className="flex items-center gap-8">
          <button onClick={prevTrack} className="text-white/40 hover:text-white transition-colors">
            <SkipBack className="w-5 h-5 fill-current" />
          </button>
          <motion.button 
            whileHover={{ scale: 1.1 }}
            whileTap={{ scale: 0.9 }}
            onClick={togglePlay}
            className="w-12 h-12 bg-white text-black rounded-full flex items-center justify-center hover:scale-105 active:scale-95 transition-all shadow-xl shadow-white/10"
          >
            {isPlaying ? <Pause className="w-5 h-5 fill-current" /> : <Play className="w-5 h-5 fill-current ml-1" />}
          </motion.button>
          <button onClick={nextTrack} className="text-white/40 hover:text-white transition-colors">
            <SkipForward className="w-5 h-5 fill-current" />
          </button>
        </div>
        
        <div className="w-full max-w-md flex items-center gap-3 group">
          <span className="text-xs font-mono text-white/40 font-bold w-12 text-right">{formatTime(progress)}</span>
          <div 
            className="flex-1 h-1.5 bg-white/5 rounded-full relative overflow-hidden cursor-pointer"
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
            onChange={onVolumeChange}
            className="w-24 h-6"
          />
        </div>
        <button 
          className="p-2 glass hover:bg-white/10 rounded-xl transition-all"
          onClick={onOpenFullscreen}
        >
          <Maximize2 className="w-4 h-4 text-white/60" />
        </button>
      </div>
    </motion.div>
  );
};
