import React from 'react';
import ReactDOM from 'react-dom/client';
import { Route, Routes, HashRouter } from 'react-router-dom';

import ReaderScreen from './screens/ReaderScreen.js';

import './index.css';
import App from './App';


const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <HashRouter>
        <Routes>
          <Route element={<App />} >
            <Route path="/" element={<ReaderScreen />} />
          </Route>
        </Routes>
      </HashRouter>
  </React.StrictMode>
);
