import React from 'react';
import { motion } from 'motion/react';
import { User } from 'lucide-react';

interface ArtistsViewProps {
  artists: string[];
}

export function ArtistsView({ artists }: ArtistsViewProps) {
  return (
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
  );
}
