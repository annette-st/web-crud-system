import React from 'react';
import './App.css';
import Header from "./components/Header/Header";
import RoutesInfo from "./components/AppContent/RoutesInfo/RoutesInfo";

const App: React.FC = () => {
    return (
        <div className="appWrapper">
            <Header/>
            <RoutesInfo/>
        </div>
    )
}

export default App;
