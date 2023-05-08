import React from 'react';
import { Outlet } from 'react-router-dom';

import MainNavbar from './components/MainNavbar';

import './App.css';

function App() {

  return (
    <div className="App">
      <header>
        <MainNavbar />
      </header>
      <main>
        <Outlet />
      </main>
      <footer>
        {/* <MainFooter /> */}
      </footer>
    </div>
  );
}

export default App;
