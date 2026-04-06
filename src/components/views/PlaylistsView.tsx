import React from 'react';
import { motion } from 'motion/react';
import { ListMusic } from 'lucide-react';
import { Playlist } from '../../services/jellyfin';

interface PlaylistsViewProps {
  playlists: Playlist[];
  onPlaylistClick: (playlist: Playlist) => void;
}

export function PlaylistsView({ playlists, onPlaylistClick }: PlaylistsViewProps) {
  return (
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
              onClick={() => onPlaylistClick(playlist)}
            >
              <div className="aspect-square glass rounded-[32px] flex items-center justify-center group-hover:scale-105 transition-all duration-500 shadow-2xl overflow-hidden relative">
                <div className="absolute inset-0 bg-gradient-to-br from-purple-600/20 to-blue-500/20" />
                <ListMusic className="w-16 h-16 text-white/20 group-hover:text-white/40 transition-colors" />
              </div>
              <div className="mt-5 px-3">
                <h3 className="font-bold truncate text-sm tracking-tight">{playlist.Name}</h3>
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
          <p className="text-white/40 max-w-xs">Suas playlists do Jellyfin aparecerão aqui.</p>
        </div>
      )}
    </motion.div>
  );
}
