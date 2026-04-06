import React from 'react';
import { motion } from 'motion/react';
import { Heart } from 'lucide-react';
import { Track } from '../../services/jellyfin';
import { TrackCard } from '../TrackCard';

interface FavoritesViewProps {
  favoriteTracks: Track[];
  playTrack: (track: Track) => void;
  albumArtStyle: 'square' | 'round';
  toggleFavorite: (track: Track) => void;
}

export function FavoritesView({ 
  favoriteTracks, 
  playTrack, 
  albumArtStyle, 
  toggleFavorite 
}: FavoritesViewProps) {
  return (
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
  );
}
