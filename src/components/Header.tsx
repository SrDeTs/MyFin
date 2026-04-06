import React from 'react';
import { Search, Disc, User } from 'lucide-react';
import { cn } from '../lib/utils';
import { ThemeSwitch } from './ui/ThemeSwitch';

interface HeaderProps {
  searchQuery: string;
  setSearchQuery: (query: string) => void;
  isDark: boolean;
  setIsDark: (dark: boolean) => void;
  loadTracks: () => void;
  isRefreshing: boolean;
  user: any;
}

export function Header({ 
  searchQuery, 
  setSearchQuery, 
  isDark, 
  setIsDark, 
  loadTracks, 
  isRefreshing, 
  user 
}: HeaderProps) {
  return (
    <header className="p-8 flex items-center justify-between z-10">
      <div className="relative w-96 group">
        <Search className="absolute left-4 top-1/2 -translate-y-1/2 w-4 h-4 text-white/30 group-focus-within:text-purple-400 transition-colors" />
        <input 
          type="text" 
          placeholder="Buscar músicas, álbuns..."
          className="w-full bg-white/5 border border-white/10 rounded-full py-2.5 pl-12 pr-4 focus:outline-none focus:ring-2 focus:ring-purple-500/30 transition-all text-sm hover:bg-white/10"
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
        />
      </div>
      <div className="flex items-center gap-6">
        <ThemeSwitch isDark={isDark} onToggle={() => setIsDark(!isDark)} />
        <button 
          onClick={loadTracks}
          disabled={isRefreshing}
          className={cn(
            "p-2.5 rounded-full glass hover:bg-white/10 transition-all active:scale-90",
            isRefreshing && "animate-spin text-purple-400"
          )}
        >
          <Disc className="w-4 h-4" />
        </button>
        <div className="flex items-center gap-4">
          <div className="text-right hidden sm:block">
            <p className="text-[10px] font-bold text-white/30 uppercase tracking-[0.2em]">Servidor</p>
            <p className="text-sm font-semibold text-white/80">Online</p>
          </div>
          <div className="w-10 h-10 rounded-2xl bg-gradient-to-br from-white/10 to-white/5 border border-white/10 flex items-center justify-center shadow-inner">
            <User className="w-5 h-5 text-white/60" />
          </div>
        </div>
      </div>
    </header>
  );
}
