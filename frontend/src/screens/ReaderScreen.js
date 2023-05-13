import React, { useEffect, useState } from 'react';
import { QrReader } from "react-qr-reader";

function updateTable(listaLinksQRCode){
  console.log("Entrou no update")
  if (document.getElementById("main-table--URL-QRCodeList") !== null) 
  {
      console.log(typeof document.getElementById("main-table--URL-QRCodeList"))
      document.getElementById("main-table--URL-QRCodeList").outerHTML = "";
  }

  var objTableQrCodeList = document.createElement("table");
  objTableQrCodeList.setAttribute("id", "main-table--URL-QRCodeList");

  var objTBodyQrCodeList = document.createElement("tbody");
  objTBodyQrCodeList.setAttribute("id", "tBodyQRCodeList");


  for (let i = 0; i < listaLinksQRCode.length; i++)
  {
      if (i % 2 == "0") 
      { // par
        var filhoTr = document.createElement("tr");
        filhoTr.setAttribute("id", "trQrCode-"+(i + 1));

        var filhoTdURL = document.createElement("td");
        filhoTdURL.classList.add("-bgCian");
        
        var filhoTdParagraph = document.createElement("p");
        filhoTdParagraph.classList.add("-bgCian");
        filhoTdParagraph.appendChild(document.createTextNode(listaLinksQRCode[i]));
        filhoTdURL.appendChild(filhoTdParagraph);

        var filhoTdLink = document.createElement("td");
        filhoTdLink.classList.add("-bgCian");

        var filhoTdAncor = document.createElement("a");
        filhoTdAncor.setAttribute("target", "_blank");
        filhoTdAncor.setAttribute("href", listaLinksQRCode[i]);

        var filhoTdButton = document.createElement("button");
        filhoTdButton.appendChild(document.createTextNode("Abrir Link"));
        filhoTdButton.classList.add("-buttonLinkGenerated");

        filhoTdAncor.appendChild(filhoTdButton);

        filhoTdLink.appendChild(filhoTdAncor);
      }
      else 
      { // impar 007bff azul bunitu

        var filhoTr = document.createElement("tr");
        filhoTr.setAttribute("id", "trQrCode-"+(i + 1));

        var filhoTdURL = document.createElement("td");
        filhoTdURL.classList.add("-bgWhite");
        
        var filhoTdParagraph = document.createElement("p");
        filhoTdParagraph.classList.add("-bgWhite");
        filhoTdParagraph.appendChild(document.createTextNode(listaLinksQRCode[i]));
        filhoTdURL.appendChild(filhoTdParagraph);

        var filhoTdLink = document.createElement("td");
        filhoTdLink.classList.add("-bgWhite");

        var filhoTdAncor = document.createElement("a");
        filhoTdAncor.setAttribute("target", "_blank");
        filhoTdAncor.setAttribute("href", listaLinksQRCode[i]);

        var filhoTdButton = document.createElement("button");
        filhoTdButton.appendChild(document.createTextNode("Abrir Link"));
        filhoTdButton.classList.add("-buttonLinkGenerated");

        filhoTdAncor.appendChild(filhoTdButton);

        filhoTdLink.appendChild(filhoTdAncor);
      }

      filhoTr.appendChild(filhoTdURL);
      filhoTr.appendChild(filhoTdLink);

      objTBodyQrCodeList.appendChild(filhoTr);

      objTableQrCodeList.appendChild(objTBodyQrCodeList);
  }

  document.getElementById("table-URL--container").appendChild(objTableQrCodeList);    
}

const Render = (props) => {
  const getLocalStorage = () => JSON.parse(localStorage.getItem('db_URL_QRCodeList')) ?? [];
  const setLocalStorage = (dbUrlQRCodeList) => localStorage.setItem("db_URL_QRCodeList", JSON.stringify(dbUrlQRCodeList));

  // CRUD 

  const readQRCodeURLStorage = () => getLocalStorage();

  const deleteQRCodeURLStorage = (index) => {
      const dbUrlQRCodeList = readQRCodeURLStorage();
      dbUrlQRCodeList.splice(index, 1);
      setLocalStorage(dbUrlQRCodeList);
  }

  const updateQRCodeURLStorage = (index, url) => {
      const dbUrlQRCodeList = readQRCodeURLStorage();
      dbUrlQRCodeList[index] = url;
      setLocalStorage(dbUrlQRCodeList);
  }


  const createQRCodeURLStorage = (url) => {
      const dbUrlQRCodeList = getLocalStorage();
      dbUrlQRCodeList.push(url);
      setLocalStorage(dbUrlQRCodeList);
  }


  const [data, setData] = useState([]);
  useEffect(() => {

    updateTable(readQRCodeURLStorage());
    console.log("Chamou o useEffect!");
    console.log("Data no useEffect: ");
    console.log(data);

  },[data])


  return (
    <>
      <QrReader
        onResult={(result, error) => {
          if (!!result) {

            if (readQRCodeURLStorage().length == 0) {
              createQRCodeURLStorage(result.getText());

              console.log("lista localstorage: ");
              console.log(readQRCodeURLStorage());

              var newData = [];
              newData.push(result.getText());
              setData(newData);

              console.log("Lista do data após setData: ");
              console.log(data);

            } else if (readQRCodeURLStorage().length > 0 && !readQRCodeURLStorage().find(url => url === result.getText())) {

              createQRCodeURLStorage(result.getText());

              var newData = [];
              newData.concat(data);
              console.log("Lista após concat:");
              console.log(newData);
              newData.push(result.getText());
              console.log("lista após append: ");
              console.log(newData);
              setData(newData);
              console.log("Lista após setData: ");
              console.log(data);
            }
          }

          if (!!error) {
            //console.info(error);
          }
        }}
        constraints={{facingMode:"user"}}
        style={{ width: '100%' }}
      />
      <section id="table-URL--container">
        {/* <p>{data.map(item => <li>{item}</li>)}</p> */}
      </section>
      
    </>
  );
};

function RenderScreen() {

  return (
    <div className="main-render--container">
      <Render />
    </div>
  );
}

export default RenderScreen;
