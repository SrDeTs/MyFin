import React from 'react';
import { motion } from 'motion/react';
import { Sparkles, X } from 'lucide-react';

interface AIInsightCardProps {
  insight: string | null;
  onClose: () => void;
}

export const AIInsightCard: React.FC<AIInsightCardProps> = ({ insight, onClose }) => {
  if (!insight) return null;

  return (
    <motion.div 
      initial={{ opacity: 0, y: 10 }}
      animate={{ opacity: 1, y: 0 }}
      className="mt-8 p-6 glass rounded-3xl border border-purple-500/30 max-w-md relative group"
    >
      <button 
        onClick={onClose}
        className="absolute top-4 right-4 text-white/20 hover:text-white transition-colors"
      >
        <X className="w-4 h-4" />
      </button>
      <div className="flex items-center gap-2 mb-3 text-purple-400">
        <Sparkles className="w-4 h-4" />
        <span className="text-[10px] font-bold uppercase tracking-[0.2em]">Curiosidade da IA</span>
      </div>
      <p className="text-sm leading-relaxed text-white/80 italic">"{insight}"</p>
    </motion.div>
  );
};
