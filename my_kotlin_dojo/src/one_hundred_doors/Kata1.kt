package one_hundred_doors

import one_hundred_doors.Kata1.*

/**
 * 練習一.
 */
class Kata1 {
    private final val NUMBER_OF_DOORS = 100

    val doors: Array<Door> = Array<Door>(NUMBER_OF_DOORS, init = {i -> Door(i + 1)})

    /**
     * 代表「門」的類別.
     */
    class Door (var id: Int = 0, var isOpenedFlag: Boolean = false)  {
        /**
         * 是否開啟.
         */
        val isOpened: Boolean
            get() = this.isOpenedFlag
        /**
         * 根據目前狀況，關門或開門.
         */
        fun toggle() {
            this.isOpenedFlag = !this.isOpenedFlag
        }
    }

    /**
     * 走過所有的門,並執行關或開.
     */
    fun walk(times: Int) {
        for(i in 1..times) {
            for (j in i..NUMBER_OF_DOORS step i) {
                doors[j - 1].toggle()
            }
        }
    }

}

fun main(args: Array<String>) {
    System.out.println("Hello World")
    var names: Array<String> = Array(10, {i -> "name" + i})
    names.forEach({ s -> println(s)})
}