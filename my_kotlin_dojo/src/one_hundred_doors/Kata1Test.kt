package one_hundred_doors

import org.junit.Assert.*
import org.junit.Test

class Kata1Test {
    var instance: Kata1 = Kata1()
    @org.junit.Before
    fun setUp() {
    }

    @org.junit.After
    fun tearDown() {
    }

    @Test
    fun test_1st_time() {
        instance.walk(1)
        assertEquals(100, instance.doors.size)
        assertTrue(instance.doors.get(0).isOpened)
        assertTrue(instance.doors.get(30).isOpened)
        assertTrue(instance.doors.get(99).isOpened)
    }

    @Test
    fun test_2nd_time() {
        instance.walk(2)
        assertEquals(100, instance.doors.size)
        assertTrue(instance.doors.get(0).isOpened)
        assertFalse(instance.doors.get(1).isOpened)
        assertTrue(instance.doors.get(2).isOpened)
        assertFalse(instance.doors.get(3).isOpened)
        assertTrue(instance.doors.get(98).isOpened)
        assertFalse(instance.doors.get(99).isOpened)
    }

    @Test
    fun test_3rd_time() {
        instance.walk(3)
        assertEquals(100, instance.doors.size)
        assertTrue(instance.doors.get(0).isOpened)
        assertFalse(instance.doors.get(1).isOpened)
        assertFalse(instance.doors.get(2).isOpened)
        assertFalse(instance.doors.get(3).isOpened)
        assertTrue(instance.doors.get(4).isOpened)
        assertTrue(instance.doors.get(5).isOpened)
    }

    @Test
    fun test_100_time() {
        instance.walk(100)
        assertTrue(instance.doors.get(0).isOpened)
        assertTrue(instance.doors.get(3).isOpened)
        assertTrue(instance.doors.get(8).isOpened)
        assertTrue(instance.doors.get(15).isOpened)
        assertTrue(instance.doors.get(24).isOpened)
    }
}