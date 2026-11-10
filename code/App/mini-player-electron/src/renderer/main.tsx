// src/renderer/main.tsx —— React 渲染入口

import React from 'react';
import { createRoot } from 'react-dom/client';
import App from './App';
import './App.css';

const rootEl = document.getElementById('root');
if (rootEl) {
  createRoot(rootEl).render(
    <React.StrictMode>
      <App />
    </React.StrictMode>
  );
}
