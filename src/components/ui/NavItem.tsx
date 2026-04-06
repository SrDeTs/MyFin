import React from 'react';
import { motion } from 'motion/react';
import { cn } from '../../lib/utils';

interface NavItemProps {
  icon: React.ReactNode;
  label: string;
  active?: boolean;
  onClick?: () => void;
}

export const NavItem: React.FC<NavItemProps> = ({ 
  icon, 
  label, 
  active = false, 
  onClick 
}) => (
  <button 
    onClick={onClick}
    className={cn(
      "w-full flex items-center gap-3 px-4 py-3 rounded-xl transition-all duration-500 group relative overflow-hidden",
      active 
        ? "bg-white/10 text-white shadow-[0_0_20px_rgba(255,255,255,0.05)]" 
        : "text-white/40 hover:text-white hover:bg-white/5"
    )}
  >
    {active && (
      <motion.div 
        layoutId="nav-active"
        className="absolute left-0 w-1 h-5 bg-purple-500 rounded-r-full"
      />
    )}
    <div className={cn("transition-transform duration-500", active ? "scale-110" : "group-hover:scale-110")}>
      {icon}
    </div>
    <span className="text-sm font-medium">{label}</span>
  </button>
);
