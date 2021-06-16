import React from 'react'
import s from '../../../CreateTableConfiguration.module.css'
import img from "../../../../../../assets/step2Final-animation.png";
import icnDone from "../../../../../../assets/icn-done.svg";

const Step2Final = (props) => {

    return (
        <div className={s.generalStepContainer}>
            <p className={s.stepNumber}>Step 2 of 3</p>
            <div className={s.stepContainer}>
                <p className={s.notes}>А. Select a table header <img src={icnDone} alt="icn-done"/></p>
                <p className={s.notes}>B. Select a table cell <img src={icnDone} alt="icn-done"/></p>
            </div>
            <img src={img} className={s.animation} alt="icn-animation"/>
            <div className={s.navButtons}>
                <button onClick={props.prev} className={s.prevButton}>Previous</button>
                <button onClick={props.next} className={s.nextButton}>Next</button>
            </div>
        </div>
    )
}

export default Step2Final
