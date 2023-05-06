import React, { useState } from 'react';
import { QrReader } from "react-qr-reader";

import './App.css';


const Render = (props) => {
  const [data, setData] = useState('No result');

  return (
    <>
      <QrReader
        onResult={(result, error) => {
          if (!!result) {
            setData(result?.text);
            console.log(result.getText())
          }

          if (!!error) {
            console.info(error);
          }
        }}
        constraints={{facingMode:"user"}}
        style={{ width: '100%' }}
      />
      <p>{data}</p>
    </>
  );
};

function App() {

  return (
    <div className="main-render--container">
      <Render />
    </div>
  );
}

export default App;
