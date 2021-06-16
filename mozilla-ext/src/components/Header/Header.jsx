import React, {useEffect} from 'react'
import s from './Header.module.css'

const Header = (props) => {
    useEffect(() => {
        let baseMouseX, baseMouseY

        const header = document.getElementById("header")

        header.addEventListener('mousedown', handleDragStart)

        function handleDragStart (evt) {
            baseMouseX = evt.clientX
            baseMouseY = evt.clientY

            window.parent.postMessage({
                msg: 'IFRAME_DRAG_START',
                mouseX: baseMouseX,
                mouseY: baseMouseY
            }, '*')

            document.addEventListener('mouseup', handleDragEnd)
            document.addEventListener('mousemove', handleMousemove)
        }

        function handleMousemove (evt) {
            window.parent.postMessage({
                msg: 'IFRAME_DRAG_MOUSEMOVE',
                offsetX: evt.clientX - baseMouseX,
                offsetY: evt.clientY - baseMouseY
            }, '*')
        }

        function handleDragEnd () {
            window.parent.postMessage({
                msg: 'IFRAME_DRAG_END'
            }, '*')

            document.removeEventListener('mouseup', handleDragEnd)
            document.removeEventListener('mousemove', handleMousemove)
        }
    })

    return (
        <header className={s.header} id="header">
            <div className={s.logoWrapper}>
                <p><img src="https://img.icons8.com/material/16/000000/puzzle--v1.png"/>
                    Web CRUD Extension
                </p>
            </div>
        </header>
    )
}

export default Header
