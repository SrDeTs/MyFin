import React, { useEffect } from 'react';
import { motion, useSpring } from 'motion/react';
import { cn } from '../lib/utils';

interface SynesthesiaBackgroundProps {
  data: number[];
  accentColor: string;
  zIndex?: string;
  enabled?: boolean;
}

export const SynesthesiaBackground: React.FC<SynesthesiaBackgroundProps> = ({ 
  data, 
  accentColor, 
  zIndex = "-z-10", 
  enabled = true 
}) => {
  const springConfig = { damping: 40, stiffness: 100, mass: 1 };
  const smoothedBass = useSpring(0, springConfig);
  const smoothedTreble = useSpring(0, springConfig);
  const smoothedAvg = useSpring(0, springConfig);

  useEffect(() => {
    if (!enabled) return;
    const avg = data.reduce((a, b) => a + b, 0) / (data.length || 1);
    const bass = data.slice(0, 5).reduce((a, b) => a + b, 0) / 5;
    const treble = data.slice(30, 40).reduce((a, b) => a + b, 0) / 10;
    
    // Scale values for better impact and less flicker
    smoothedBass.set(0.8 + bass * 1.5);
    smoothedTreble.set(0.5 + treble * 2);
    smoothedAvg.set(0.3 + avg * 0.7);
  }, [data, enabled, smoothedBass, smoothedTreble, smoothedAvg]);

  if (!enabled) return null;

  return (
    <div className={cn("fixed inset-0 overflow-hidden pointer-events-none", zIndex)}>
      {/* Layer 1: Main Bass Aura */}
      <motion.div 
        style={{ 
          scale: smoothedBass,
          opacity: smoothedAvg,
          willChange: "transform, opacity"
        }}
        className={cn(
          "absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[180vw] h-[180vw] rounded-full blur-[180px] transition-colors duration-1000",
          accentColor === 'purple' && "bg-purple-500/90",
          accentColor === 'blue' && "bg-blue-500/90",
          accentColor === 'green' && "bg-green-500/90",
          accentColor === 'pink' && "bg-pink-500/90",
          accentColor === 'orange' && "bg-orange-500/90"
        )}
      />
      
      {/* Layer 2: Treble Highlights */}
      <motion.div 
        style={{ 
          scale: smoothedTreble,
          opacity: smoothedTreble,
          x: smoothedTreble,
          y: smoothedTreble,
          willChange: "transform, opacity"
        }}
        className="absolute top-1/4 left-1/4 w-[120vw] h-[120vw] bg-white/50 rounded-full blur-[160px]"
      />

      {/* Layer 3: Accent Pulse */}
      <motion.div 
        style={{ 
          scale: smoothedAvg,
          opacity: smoothedBass,
          willChange: "transform, opacity"
        }}
        className={cn(
          "absolute bottom-0 right-0 w-[100vw] h-[100vw] rounded-full blur-[200px] transition-colors duration-1000",
          accentColor === 'purple' && "bg-indigo-600/70",
          accentColor === 'blue' && "bg-cyan-600/70",
          accentColor === 'green' && "bg-emerald-600/70",
          accentColor === 'pink' && "bg-rose-600/70",
          accentColor === 'orange' && "bg-amber-600/70"
        )}
      />
    </div>
  );
};
