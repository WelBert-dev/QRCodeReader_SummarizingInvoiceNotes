import React, { useState } from 'react';

function WriterScreen() {
  
  const [msg, setMsg] = useState('');

  const handleSubmit = (event) => {
    event.preventDefault();
    console.log("ENTROU NO HANDLE SUBMIT");
    console.log("MSG: ");
    console.log(msg);
    const url = `http://localhost:8000/api/post`;
    fetch(url, {
      method: 'POST',
      mode: 'no-cors',
      headers: {
        "Content-type": "application/json; charset=UTF-8"
      },
      body: JSON.stringify({ msg: msg })
    })
    .then(response => response.arrayBuffer())
    .then((buffer) => {
      console.log("RESPONSE: ");
      console.log(buffer);
      const blob = new Blob([buffer], {type: 'image/png'});
      const imgUrl = URL.createObjectURL(blob);
      const img = new Image();
      img.onload = function() {
      // exibir a imagem em uma tag img no DOM
      document.body.appendChild(img);
      };
      img.src = imgUrl;
    })
    .catch(error => console.error(error));

    setMsg('');
  };

  const handleChange = (event) => {
    setMsg(event.target.value);
  };

  return (
    <div className="main-writer--container">

      <form action="127.0.0.1:8000/api/post" method="POST"
       onSubmit={handleSubmit}>
        <label>Mensagem: </label>
        <input type="text" name="msg" value={msg} onChange={handleChange}></input><br/>

        <input type="submit" value="Enviar"></input>
        <input type="reset" value="Limpar"></input>

      </form>
    </div>
  );
}

export default WriterScreen;
