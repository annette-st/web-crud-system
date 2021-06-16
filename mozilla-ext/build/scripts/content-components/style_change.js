export function addCssHighlightForTag(elems) {
    let els = document.querySelectorAll(elems)
    for (let el of els) {
        $(el).hover(
            function () {
                el.style.border = '2px solid #3496F7'
            },
            function () {
                el.style.border = ''
            }
        )
    }
    console.log('highlightElements')
}
export function removeCssHighlightForTag(elems) {
    let els = document.querySelectorAll(elems)
    for (let el of els) {
        $(el).hover(
            function () {
                el.style.border = ''
            },
            function () {
                el.style.border = ''
            }
        )
    }
}
export function removeCssHighlightForClass(className) {
    let els = document.getElementsByClassName(className)
    console.log(els)
    for (let i = 0; i < els.length; i++) {
        els[i].style.border = ''
    }
}
