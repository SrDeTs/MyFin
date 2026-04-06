import React, { useState, useEffect, useRef } from 'react';
import { cn } from '../../lib/utils';

interface PlasmaVolumeProps {
  value: number;
  onChange: (v: number) => void;
  orientation?: 'horizontal' | 'vertical';
  className?: string;
}

export const PlasmaVolume: React.FC<PlasmaVolumeProps> = ({ 
  value, 
  onChange, 
  orientation = 'horizontal',
  className 
}) => {
  const containerRef = useRef<HTMLDivElement>(null);
  const [isDragging, setIsDragging] = useState(false);

  const handleInteraction = (e: React.MouseEvent | React.TouchEvent | MouseEvent | TouchEvent) => {
    if (!containerRef.current) return;
    const rect = containerRef.current.getBoundingClientRect();
    let newValue: number;

    if (orientation === 'horizontal') {
      const clientX = 'touches' in e ? (e as TouchEvent).touches[0].clientX : (e as MouseEvent).clientX;
      const x = Math.max(0, Math.min(clientX - rect.left, rect.width));
      newValue = x / rect.width;
    } else {
      const clientY = 'touches' in e ? (e as TouchEvent).touches[0].clientY : (e as MouseEvent).clientY;
      const y = Math.max(0, Math.min(clientY - rect.top, rect.height));
      newValue = 1 - (y / rect.height);
    }
    onChange(newValue);
  };

  useEffect(() => {
    const handleMove = (e: MouseEvent | TouchEvent) => {
      if (isDragging) {
        handleInteraction(e);
      }
    };
    const handleUp = () => setIsDragging(false);

    if (isDragging) {
      window.addEventListener('mousemove', handleMove);
      window.addEventListener('mouseup', handleUp);
      window.addEventListener('touchmove', handleMove);
      window.addEventListener('touchend', handleUp);
    }
    return () => {
      window.removeEventListener('mousemove', handleMove);
      window.removeEventListener('mouseup', handleUp);
      window.removeEventListener('touchmove', handleMove);
      window.removeEventListener('touchend', handleUp);
    };
  }, [isDragging]);

  return (
    <div 
      ref={containerRef}
      className={cn(
        "plasma-container relative cursor-pointer",
        orientation === 'horizontal' ? "w-32 h-6" : "w-10 h-48",
        className
      )}
      onMouseDown={(e) => {
        setIsDragging(true);
        handleInteraction(e);
      }}
      onTouchStart={(e) => {
        setIsDragging(true);
        handleInteraction(e);
      }}
    >
      <div className="plasma-track">
        {/* Ticks */}
        <div className={cn(
          "plasma-ticks absolute inset-0 px-2",
          orientation === 'horizontal' ? "flex-row py-1" : "flex-col py-2"
        )}>
          {[...Array(8)].map((_, i) => (
            <div 
              key={i} 
              className={cn(
                "plasma-tick",
                orientation === 'horizontal' ? "w-[1px] h-full" : "h-[1px] w-full",
                (i / 7) <= value && "active"
              )} 
            />
          ))}
        </div>

        {/* Fill */}
        <div className="plasma-fill-container">
          <div 
            className="plasma-fill absolute"
            style={{
              ...(orientation === 'horizontal' 
                ? { left: 0, bottom: 0, top: 0, width: `${value * 100}%` }
                : { left: 0, right: 0, bottom: 0, height: `${value * 100}%` }
              )
            }}
          />
        </div>

        {/* Knob */}
        <div 
          className="plasma-knob"
          style={{
            width: orientation === 'horizontal' ? '20px' : '28px',
            height: orientation === 'horizontal' ? '20px' : '28px',
            ...(orientation === 'horizontal'
              ? { left: `${value * 100}%`, top: '50%', transform: 'translate(-50%, -50%)' }
              : { left: '50%', bottom: `${value * 100}%`, transform: 'translate(-50%, 50%)' }
            )
          }}
        />
      </div>
    </div>
  );
};
