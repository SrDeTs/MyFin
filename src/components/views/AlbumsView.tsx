import React from 'react';
import { motion } from 'motion/react';
import { Disc } from 'lucide-react';

interface AlbumsViewProps {
  albums: string[];
}

export function AlbumsView({ albums }: AlbumsViewProps) {
  return (
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
  );
}
