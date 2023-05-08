import React, { useRef, useState, useEffect } from 'react';
import { FaBars, FaTimes } from 'react-icons/fa';
import { useLocation } from 'react-router-dom';

import './MainNavbar.css';

export default function MainNavbar() {
    const navRef = useRef();

    const [isShowNavbarResponsive, setIsShowNavbarResponsive] = useState(false);

    const showNavbar = () => {
        
        if (navRef.current.classList.toggle("responsive_nav"))
        { // responsivo ativo e botao precionado
            document.getElementById("btn-hamburguer").style.display = 'none';
            document.getElementById("btn-hamburguer").style.position = 'relative';
            document.getElementById("navbar-container").classList.remove("responsive_nav-opacityZero");
            document.getElementById("navbar-container").classList.add("responsive_nav-opacityUm");
            // if(window.screen.width >= 810){
            //     document.getElementById("navbar-container").style.flexDirection = "column";
            //     document.getElementById("navbar-container").style.width = "inherit";
            //     console.log("Entrou no if do if");
            // }
            // setIsShowNavbarResponsive(true);
            console.log("entrou no if");
        }else 
        {
            document.getElementById("btn-hamburguer").style.display = 'block';
            document.getElementById("btn-hamburguer").style.position = 'fixed';
            document.getElementById("navbar-container").classList.remove("responsive_nav-opacityUm");
            document.getElementById("navbar-container").classList.add("responsive_nav-opacityZero");
            // if(window.screen.width >= 810){
            //     document.getElementById("navbar-container").style.flexDirection = "column";
            //     document.getElementById("navbar-container").style.width = "inherit";
            //     console.log("Entrou no if do else");
            // }
            // setIsShowNavbarResponsive(false);
            console.log("else");
        }  
    };

    const locationFlag = useLocation();
    const [navBarEmColumn, setNavBarEmColumn] = useState(false);

    useEffect(()=>{
        console.log("Entrou no useEffect");
        if(window.screen.width >= 810 && window.location.href.substring(1).split("/")[5] == 'projects' || window.screen.width >= 810 && window.location.href.substring(1).split("/")[5] == 'certificates'){
            const navBarElement = document.getElementById("navbar-container");
            navBarElement.style.flexDirection = "column";
            navBarElement.style.width = "inherit"; 
    
            if (window.location.href.substring(1).split("/")[5] == 'projects' && window.screen.width >= 810){
                const sectionElement = document.getElementById("main-projects--container");
                sectionElement.style.padding = "2rem 0 1rem 10%";
            }else if(window.location.href.substring(1).split("/")[5] == 'certificates' && window.screen.width >= 810){
                const sectionElement = document.getElementById("main-certificate--container");
                sectionElement.style.padding = "2rem 0 1rem 10%";
            }
            
        }else{
            console.log(window.screen.width);
            if(window.screen.width >= 810){
                const navBarElement = document.getElementById("navbar-container");
                navBarElement.style.flexDirection = "row";
                navBarElement.style.width = "100%"; 
                console.log("navRef responsiva cu: ", navRef.current.classList.contains("responsive_nav"));
            }
        }
    }, [navBarEmColumn, locationFlag]);

    return (
        <>
            <nav id="navbar-container" ref={navRef} >
                <button id="nav-close-btn" className="nav-btn nav-close-btn" onClick={showNavbar}><FaTimes /></button>
                <a href="/">Reader</a>
                <a href="/#/writer" onClick={() => setNavBarEmColumn(true)}>Writer</a>
            </nav>
            <button id="btn-hamburguer" className="nav-btn" onClick={showNavbar}><FaBars /></button>
        </>
        

    )
};