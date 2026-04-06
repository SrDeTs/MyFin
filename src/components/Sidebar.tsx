import React from 'react';
import { Music, LayoutGrid, Disc, User, Heart, ListMusic, Settings } from 'lucide-react';
import { cn } from '../lib/utils';
import { NavItem } from './ui/NavItem';

interface SidebarProps {
  immersiveMode: boolean;
  isSettingsOpen: boolean;
  activeView: 'explore' | 'albums' | 'artists' | 'favorites' | 'playlists';
  setActiveView: (view: 'explore' | 'albums' | 'artists' | 'favorites' | 'playlists') => void;
  setIsSettingsOpen: (open: boolean) => void;
}

export function Sidebar({ 
  immersiveMode, 
  isSettingsOpen, 
  activeView, 
  setActiveView, 
  setIsSettingsOpen 
}: SidebarProps) {
  return (
    <aside className={cn(
      "w-64 glass-dark flex flex-col p-6 pb-32 z-20 border-r border-white/5 transition-all duration-500",
      immersiveMode && !isSettingsOpen ? "-translate-x-full opacity-0 w-0 p-0 overflow-hidden" : "translate-x-0 opacity-100"
    )}>
      <div className="flex items-center gap-3 mb-10 px-2">
        <div className="w-10 h-10 bg-gradient-to-tr from-purple-600 to-blue-500 rounded-xl flex items-center justify-center shadow-lg shadow-purple-500/20">
          <Music className="text-white w-5 h-5" />
        </div>
        <span className="text-xl font-display font-bold tracking-tight">MyFin</span>
      </div>

      <nav className="flex-1 space-y-1 overflow-y-auto custom-scrollbar pr-2">
        <NavItem 
          icon={<LayoutGrid className="w-4 h-4" />} 
          label="Explorar" 
          active={activeView === 'explore'} 
          onClick={() => setActiveView('explore')}
        />
        <NavItem 
          icon={<Disc className="w-4 h-4" />} 
          label="Álbuns" 
          active={activeView === 'albums'} 
          onClick={() => setActiveView('albums')}
        />
        <NavItem 
          icon={<User className="w-4 h-4" />} 
          label="Artistas" 
          active={activeView === 'artists'} 
          onClick={() => setActiveView('artists')}
        />
        <NavItem 
          icon={<Heart className="w-4 h-4" />} 
          label="Favoritos" 
          active={activeView === 'favorites'} 
          onClick={() => setActiveView('favorites')}
        />
        <NavItem 
          icon={<ListMusic className="w-4 h-4" />} 
          label="Playlists" 
          active={activeView === 'playlists'} 
          onClick={() => setActiveView('playlists')}
        />
      </nav>

      <div className="mt-auto pt-6 border-t border-white/5">
        <NavItem 
          icon={<Settings className="w-4 h-4" />} 
          label="Configurações" 
          onClick={() => setIsSettingsOpen(true)}
        />
      </div>
    </aside>
  );
}
