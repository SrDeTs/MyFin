import React from 'react';
import { motion } from 'motion/react';
import { Play, Heart } from 'lucide-react';
import { cn } from '../lib/utils';
import { Track, jellyfin } from '../services/jellyfin';

interface TrackCardProps {
  track: Track;
  index: number;
  onPlay: (t: Track) => void;
  albumArtStyle: 'square' | 'round';
  onToggleFavorite?: (t: Track) => void;
  isFavorite?: boolean;
}

export const TrackCard: React.FC<TrackCardProps> = ({ 
  track, 
  index, 
  onPlay, 
  albumArtStyle, 
  onToggleFavorite, 
  isFavorite 
}) => (
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
