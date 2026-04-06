import React from 'react';
import './ThemeSwitch.css'; // Assuming the styles are in a separate CSS file or global CSS

interface ThemeSwitchProps {
  isDark: boolean;
  onToggle: () => void;
}

export const ThemeSwitch: React.FC<ThemeSwitchProps> = ({ isDark, onToggle }) => {
  return (
    <label className="switch theme-switch-container">
      <input 
        className="switch__input" 
        type="checkbox" 
        role="switch" 
        checked={isDark}
        onChange={onToggle}
      />
      <span className="switch__icon">
        <span className="switch__icon-part switch__icon-part--1"></span>
        <span className="switch__icon-part switch__icon-part--2"></span>
        <span className="switch__icon-part switch__icon-part--3"></span>
        <span className="switch__icon-part switch__icon-part--4"></span>
        <span className="switch__icon-part switch__icon-part--5"></span>
        <span className="switch__icon-part switch__icon-part--6"></span>
        <span className="switch__icon-part switch__icon-part--7"></span>
        <span className="switch__icon-part switch__icon-part--8"></span>
        <span className="switch__icon-part switch__icon-part--9"></span>
        <span className="switch__icon-part switch__icon-part--10"></span>
        <span className="switch__icon-part switch__icon-part--11"></span>
      </span>
      <span className="switch__sr">Dark Mode</span>
    </label>
  );
};
