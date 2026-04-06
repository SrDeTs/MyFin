import React from 'react';
import { motion } from 'motion/react';
import { Track } from '../../services/jellyfin';
import { TrackCard } from '../TrackCard';

interface ExploreViewProps {
  filteredTracks: Track[];
  playTrack: (track: Track) => void;
  albumArtStyle: 'square' | 'round';
  toggleFavorite: (track: Track) => void;
  favoriteTracks: Track[];
}

export function ExploreView({ 
  filteredTracks, 
  playTrack, 
  albumArtStyle, 
  toggleFavorite, 
  favoriteTracks 
}: ExploreViewProps) {
  return (
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
  );
}
