import React, { useState } from 'react';

import imgTest from './httpsgithubcomWelBertdev.png';

function WriterScreen() {
  
  const [msg, setMsg] = useState(null);
  const [imageSrc, setImageSrc] = useState('');
  const [isValidImage, setIsValidImage] = useState(false);

  const handleSubmit = async (event) => {
    event.preventDefault();
    console.log("ENTROU NO HANDLE SUBMIT");
    console.log("MSG: ");
    console.log(msg);
    const url = `http://localhost:8000/api/post`;

  fetch(url, {
    method: 'POST',
    headers: {
      "Content-type": "application/json; charset=UTF-8",
      "mode": "cors",
      "Origin": "http://localhost:3000",
      "Referer": "http://localhost:3000/",
      "Access-Control-Request-Headers": "Content-type",
      "Access-Control-Request-Method": "*",
      // "Access-Control-Allow-Origin": "*",

    },
    body: JSON.stringify({ msg: msg })
  })
  .then(response => {
    if (response.ok) {
      const contentType = response.headers.get('Content-Type');

      // Verificar o tipo de conteúdo e converter conforme necessário
      if (contentType.includes('application/json')) {
        return response.json();
      } else if (contentType.includes('image/png') || contentType.includes('image/jpeg')) {
        return response.blob();
      } else {
        return response.text();
      }
    } else {
      throw new Error('Erro na solicitação: ' + response.status);
    }
  })
  .then(data => {
    // Manipular os dados conforme o tipo de conteúdo
if (data instanceof Blob) {
      // Dados da imagem
      if (URL && typeof URL.createObjectURL === 'function') {
        console.log("IMAGEM BLOB VÀLIDA ENTROU NO IF");
        const imgURL = URL.createObjectURL(data);
        // Realizar as ações necessárias com a imagem
        console.log(imgURL);
        // const imgElement = document.createElement('img');
        // imgElement.setAttribute("src", imgURL);
        setImageSrc(imgURL);

        // Verificar se o blob é uma imagem válida
        const isImage = data.type.startsWith('image/');
        setIsValidImage(isImage);
  
        // const conatinerElement = document.getElementById("main-writer--container");
        // conatinerElement.appendChild(imgElement);

      } else {
        throw new Error('O navegador não suporta a função createObjectURL.');
      }
    } else {
      // Outro tipo de dado
      console.log("OUTRO TIPO DE DADO: ");
      console.log(data);
    }
  })
  .catch(error => {
    console.error(error);
  });
    
    // await fetch(url, {
      // method: 'POST',
      //   headers: {
      //     "Content-type": "application/json; charset=UTF-8",
      //     "mode": "cors",
      //     "Origin": "http://localhost:3000",
      //     "Referer": "http://localhost:3000/",
      //     "Access-Control-Request-Headers": "content-type",
      //     "Access-Control-Request-Method": "*",

      //   },
      //   body: JSON.stringify({ msg: msg })
    // })
    // .then(response => {
    //   if (response.status == 200 && response.headers.get('Content-Type').includes("image/png")){
    //     console.log("DEU BÃO, segue RESPONSE:");
    //     console.log(response);

    //     const responseBlob = response.blob();

    //     const imgUrl = URL.createObjectURL(responseBlob);
  
        // const imgElement = document.createElement('img');
        // imgElement.src = imgUrl;
  
        // const conatinerElement = document.getElementById("main-writer--container");
        // conatinerElement.appendChild(imgElement);

    //   } else {
    //     console.log("Deu bão mai ou menu, entrou no ELSE do if response.ok, segue RESPONSE: ");
    //     console.log(response.headers.get('Content-Type'));

    //   }
    // })
    // .catch(error => {
    //   console.log("DEU RUIMZAO, caiu no catch... objeto RESPONSE Inexistente, segue error: ");
    //   console.error(error)
    // });

    // try {
    //   const response = await fetch(url, {
    //     method: 'POST',
    //     headers: {
    //       "Content-type": "application/json; charset=UTF-8",
    //       "mode": "cors",
    //       "Origin": "http://localhost:3000",
    //       "Referer": "http://localhost:3000/",
    //       "Access-Control-Request-Headers": "content-type",
    //       "Access-Control-Request-Method": "*",

    //     },
    //     body: JSON.stringify({ msg: msg })
    //   });
    //   console.log(response);
    //   const responseBlob = await response.blob();

    //   const imgUrl = URL.createObjectURL(responseBlob);

    //   const imgElement = document.createElement('img');
    //   imgElement.src = imgUrl;

    //   const conatinerElement = document.getElementById("main-writer--container");
    //   conatinerElement.appendChild(imgElement);

    //   // Adicione a imagem ao elemento desejado na página
    //   // document.body.appendChild(imgElement); 
    //   // const img = document.getElementById('my-image');
    //   // const rootProject = "./public/";


    //   // const cleanedStr = msg.replace(/[^\w\s]/gi, '');
    //   // console.log(cleanedStr);

    //   // // path da img QR gerada:
    //   // const pathOfImage = rootProject + cleanedStr + ".png";
    //   // console.log(pathOfImage);

    //   // fetch(pathOfImage, { method: 'HEAD' })
    //   //   .then(response => {
    //   //     if (response.ok) {
    //   //       // console.log("DEU BAO");

    //   //       // const objImgElement = document.createElement("img");
    //   //       // objImgElement.src = pathOfImage;

    //         // const objConatinerElement = document.getElementById("main-writer--container");
    //         // objConatinerElement.appendChild(objImgElement);
    //   //       setImageSrc(pathOfImage);
    //   //     } else {
    //   //       console.error(`Erro: o arquivo ${pathOfImage} não foi encontrado.`);
    //   //     }
    //   //   })
    //   //   .catch(error => console.error(error));
      

    // } catch (error) {
    //   console.log("ERRROOOOOOO");
    //   console.error(error);
    // }

    setMsg('');
  };

  const handleChange = (event) => {
    setMsg(event.target.value);
  };

  return (
    <div id="main-writer--container" className="main-writer--container">

      <form action="http://localhost:8000/api/post" method="POST"
       onSubmit={handleSubmit}>
        <label>Mensagem: </label>
        <input type="text" name="msg" value={msg} onChange={handleChange}></input><br/>

        <input type="submit" value="Enviar"></input>
        <input type="reset" value="Limpar"></input>

      </form>

      {/* {imageSrc && <img src={imageSrc} alt="Imagem" />} */}

      {isValidImage ? (
        <img src={imageSrc} alt="Imagem" />
      ) : (
        <p>Imagem inválida</p>
      )}
      {/* <img src={imgTest} alt="pamonha"/> */}
    </div>
  );
}

export default WriterScreen;
