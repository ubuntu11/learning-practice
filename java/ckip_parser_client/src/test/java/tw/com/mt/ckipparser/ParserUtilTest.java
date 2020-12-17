/*
 * ParserUtilTest.java    1.0 2017年5月19日
 *
 * Copyright (c) 2017-2030 Monmouth Technologies, Inc.
 * http://www.mt.com.tw
 * 10F-1 No. 306 Chung-Cheng 1st Road, Linya District, 802, Kaoshiung, Taiwan
 * All Rights Reserved.
 *
 * This software is the confidential and proprietary information of Monmouth
 * Technologies, Inc. You shall not disclose such Confidential Information and
 * shall use it only in accordance with the terms of the license agreement you
 * entered into with Monmouth Technologies.
 */
package tw.com.mt.ckipparser;

import static org.junit.Assert.*;
import javax.xml.bind.JAXBException;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.c05mic.generictree.Tree;

import tw.com.mt.ckipparser.ParserUtil;
/**
 * Unit Tests for ParserUtil class.
 *
 * @version 1.0 2017年5月19日
 * @author ken
 *
 */
public class ParserUtilTest {
    /**
     * 測試把Parser產生的結果透過Jaxb變成Java Bean.
     * @throws JAXBException if something goes wrong
     */
    @Test
    public void testXmlToResult() throws JAXBException {
        String testXml = "<?xml version=\"1.0\" ?><wordsegmentation version=\"0.1\">"
                + "<processstatus code=\"0\">Success</processstatus><result><sentence>"
                + "#1:1.[0] S(NP(N:台新|Head:N:金控)|Nd:１２月３日|ADV:將|Head:Vt:召開|"
                + "NP(N:股東|Head:N:臨時會)|VP(Head:Vt:進行|NP(NP(Head:N:董監)|Head:Nv:改選)))"
                + "#。(PERIODCATEGORY)</sentence></result></wordsegmentation>";
        ParserResult result = ParserUtil.xmlToResult(testXml);
        assertNotNull(result);
        assertEquals("Success", result.getProcessStatus());
        assertEquals(0, result.getStatusCode());
        assertTrue(result.getSentence()[0].matches("^#1:1.*PERIODCATEGORY\\)$"));
    }

    /**
     * 測試產生要提交給ckip parser的xml request.
     */
    @Test
    public void testGenerateXmlRequest() {
        String sentence = "台新金控12月3日將召開股東臨時會進行董監改選。";
        String result = ParserUtil.generateXmlRequest("ken", "12345", sentence);
        assertTrue(result.matches("^<?.*" + sentence + ".*wordsegmentation>$"));
    }

    /**
     * 測試第一個句子(手機上網如何設定?)的分析結果.
     */
    @Test
    public void testTaggedTextToTermTree() {
        String sentence = "#1:1.[0] S(NP(N:手機|Head:Nv:上網)|ADV:如何|Head:Vt:設定)#?(QUESTIONCATEGORY)";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        assertEquals("S", result.getRoot().getData().getPos());
        assertEquals(3, result.getRoot().getChildren().size());
        assertEquals("NP", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals(2, result.getRoot().getChildAt(0).getChildren().size());
        assertEquals("N",
                result.getRoot().getChildAt(0).getChildren().get(0).getData().getPos());
        assertEquals("手機", result.getRoot().getChildAt(0).getChildren().get(0).getData().getText());
        assertEquals("Head",
                result.getRoot().getChildAt(0).getChildren().get(1).getData().getThematicRole());
        assertEquals("Nv", result.getRoot().getChildAt(0).getChildren().get(1).getData().getPos());
        assertEquals("上網", result.getRoot().getChildAt(0).getChildren().get(1).getData().getText());
        assertEquals("ADV", result.getRoot().getChildAt(1).getData().getPos());
        assertEquals("如何", result.getRoot().getChildAt(1).getData().getText());
        assertEquals("Head", result.getRoot().getChildAt(2).getData().getThematicRole());
        assertEquals("Vt", result.getRoot().getChildAt(2).getData().getPos());
        assertEquals("設定", result.getRoot().getChildAt(2).getData().getText());
    }

    /**
     * 測試第二個句子(「保證金」退還作業方式為何?)的分析結果.
     */
    @Test
    public void testTaggedTextToTermTree2() {
        String sentence = "#1:1.[0] S(NP(Head:N:保證金)|Head:Vt:退還|NP(N:作業|Head:N:方式)|ADV:為何)"
                + "#?(QUESTIONCATEGORY)";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        assertEquals("S", result.getRoot().getData().getPos());
        assertEquals(4, result.getRoot().getChildren().size());
        assertEquals("NP", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals(1, result.getRoot().getChildAt(0).getChildren().size());
        assertEquals("Head",
                result.getRoot().getChildAt(0).getChildren().get(0).getData().getThematicRole());
        assertEquals("N", result.getRoot().getChildAt(0).getChildren().get(0).getData().getPos());
        assertEquals("保證金",
                result.getRoot().getChildAt(0).getChildren().get(0).getData().getText());
        assertEquals("Head", result.getRoot().getChildAt(1).getData().getThematicRole());
        assertEquals("Vt", result.getRoot().getChildAt(1).getData().getPos());
        assertEquals("退還", result.getRoot().getChildAt(1).getData().getText());
        assertEquals("NP", result.getRoot().getChildAt(2).getData().getPos());
        assertEquals(2, result.getRoot().getChildAt(2).getChildren().size());
        assertEquals("N", result.getRoot().getChildAt(2).getChildAt(0).getData().getPos());
        assertEquals("作業", result.getRoot().getChildAt(2).getChildAt(0).getData().getText());
        assertEquals("Head",
                result.getRoot().getChildAt(2).getChildAt(1).getData().getThematicRole());
        assertEquals("N", result.getRoot().getChildAt(2).getChildAt(1).getData().getPos());
        assertEquals("方式", result.getRoot().getChildAt(2).getChildAt(1).getData().getText());
    }

    /**
     * 測試第三個句子(請問台灣大哥大的日本上網漫遊卡，訊號涵蓋區域有多大?)的分析結果.
     */
    @Test
    public void testTaggedTextToTermTree3() {
        String sentence = "#1:1.[0] VP(Head:VE11:請問|theme:NP(property:NP‧的(head:NP("
                + "property:Nca:台灣|Head:Nab:大哥大)|Head:DE:的)|property:Nca:日本|"
                + "property:Nv:上網|Head:Na:漫遊卡))#，(COMMACATEGORY)";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        //getRoot()=VP()
        assertEquals("VP", result.getRoot().getData().getPos());
        assertEquals(2, result.getRoot().getChildren().size());
        //getRoot.getChildAt(0)=Head:VE11:請問
        assertEquals("VE11", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(0).getData().getThematicRole());
        assertEquals("請問", result.getRoot().getChildAt(0).getData().getText());
        //getRoot.getChildAt(1)=theme:NP()
        assertEquals("theme", result.getRoot().getChildAt(1).getData().getThematicRole());
        assertEquals("NP", result.getRoot().getChildAt(1).getData().getPos());
        assertEquals(4, result.getRoot().getChildAt(1).getChildren().size());
        //getRoot.getChildAt(1).getChildAt(0)=property:NP‧的()
        assertEquals("property",
                result.getRoot().getChildAt(1).getChildAt(0).getData().getThematicRole());
        assertEquals("NP", result.getRoot().getChildAt(1).getChildAt(0).getData().getPos());
        assertEquals(2, result.getRoot().getChildAt(1).getChildAt(0).getChildren().size());
        //getRoot.getChildAt(1).getChildAt(0)getChildAt(0)=head:NP()
        assertEquals("head", result.getRoot().getChildAt(1).getChildAt(0).getChildAt(0).getData()
                .getThematicRole());
        assertEquals("NP",
                result.getRoot().getChildAt(1).getChildAt(0).getChildAt(0).getData().getPos());
        //getRoot.getChildAt(1).getChildAt(1)=property:Nca:日本
        assertEquals("property",
                result.getRoot().getChildAt(1).getChildAt(1).getData().getThematicRole());
        assertEquals("Nca", result.getRoot().getChildAt(1).getChildAt(1).getData().getPos());
        assertEquals("日本", result.getRoot().getChildAt(1).getChildAt(1).getData().getText());
        //getRoot.getChildAt(1).getChildAt(2)=property:Nv:上網
        assertEquals("property",
                result.getRoot().getChildAt(1).getChildAt(2).getData().getThematicRole());
        assertEquals("Nv", result.getRoot().getChildAt(1).getChildAt(2).getData().getPos());
        assertEquals("上網", result.getRoot().getChildAt(1).getChildAt(2).getData().getText());
        //getRoot.getChildAt(1).getChildAt(3)=Head:Na:漫遊卡
        assertEquals("Head",
                result.getRoot().getChildAt(1).getChildAt(3).getData().getThematicRole());
        assertEquals("Na", result.getRoot().getChildAt(1).getChildAt(3).getData().getPos());
        assertEquals("漫遊卡", result.getRoot().getChildAt(1).getChildAt(3).getData().getText());
    }

    /**
     * Test case for resolving issue#1 on github with MT parser output.
     */
    @Test
    public void testTaggedTextToTermTree4A() {
        String sentence = "#1:1.[0] S(theme:NP(Head:Nhaa:你)|Head:V_12:是|range:PP("
                + "Head:P39:用|DUMMY:NP(quantifier:Nep:什麼|property:S•的(head:S("
                + "goal:NP(Head:Nac:語言)|Head:VC2:開發)|Head:DE:的))))#";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        assertEquals("S", result.getRoot().getData().getPos());
        assertEquals(3, result.getRoot().getChildren().size());
        assertEquals("NP", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals("theme", result.getRoot().getChildAt(0).getData().getThematicRole());

        assertEquals("Nhaa", result.getRoot().getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("Head",
                result.getRoot().getChildAt(0).getChildAt(0).getData().getThematicRole());
        assertEquals("你", result.getRoot().getChildAt(0).getChildAt(0).getData().getText());

        assertEquals("V_12", result.getRoot().getChildAt(1).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(1).getData().getThematicRole());
        assertEquals("是", result.getRoot().getChildAt(1).getData().getText());

        assertEquals("P39", result.getRoot().getChildAt(2).getChildAt(0).getData().getPos());
        assertEquals("Head",
                result.getRoot().getChildAt(2).getChildAt(0).getData().getThematicRole());
        assertEquals("用", result.getRoot().getChildAt(2).getChildAt(0).getData().getText());

        assertEquals("Nep",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0).getData().getPos());
        assertEquals("quantifier", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0)
                .getData().getThematicRole());
        assertEquals("什麼",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0).getData().getText());

        assertEquals("S",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getData().getPos());
        assertEquals("property", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getData().getThematicRole());

        assertEquals("Nac", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(0).getChildAt(0).getData().getThematicRole());
        assertEquals("語言", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getText());

        assertEquals("VC2", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getThematicRole());
        assertEquals("開發", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getText());

        assertEquals("DE", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(1)
                .getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(1).getData().getThematicRole());
        assertEquals("的", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(1)
                .getData().getText());
    }


    /**
     * Test case for resolving issue#1 on github with online ckip parser output.
     * The differences are "Head:V_11:是" and "S‧的".
     */
    @Test
    public void testTaggedTextToTermTree4B() {
        String sentence = "#1:1.[0] S(theme:NP(Head:Nhaa:你)|Head:V_11:是|range:PP("
                + "Head:P39:用|DUMMY:NP(quantifier:Nep:什麼|property:S‧的(head:S("
                + "goal:NP(Head:Nac:語言)|Head:VC2:開發)|Head:DE:的))))#";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        assertEquals("S", result.getRoot().getData().getPos());
        assertEquals(3, result.getRoot().getChildren().size());
        assertEquals("NP", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals("theme", result.getRoot().getChildAt(0).getData().getThematicRole());

        assertEquals("Nhaa", result.getRoot().getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("Head",
                result.getRoot().getChildAt(0).getChildAt(0).getData().getThematicRole());
        assertEquals("你", result.getRoot().getChildAt(0).getChildAt(0).getData().getText());

        assertEquals("V_11", result.getRoot().getChildAt(1).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(1).getData().getThematicRole());
        assertEquals("是", result.getRoot().getChildAt(1).getData().getText());

        assertEquals("P39", result.getRoot().getChildAt(2).getChildAt(0).getData().getPos());
        assertEquals("Head",
                result.getRoot().getChildAt(2).getChildAt(0).getData().getThematicRole());
        assertEquals("用", result.getRoot().getChildAt(2).getChildAt(0).getData().getText());

        assertEquals("Nep",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0).getData().getPos());
        assertEquals("quantifier", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0)
                .getData().getThematicRole());
        assertEquals("什麼",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(0).getData().getText());

        assertEquals("S",
                result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getData().getPos());
        assertEquals("property", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getData().getThematicRole());

        assertEquals("Nac", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(0).getChildAt(0).getData().getThematicRole());
        assertEquals("語言", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getText());

        assertEquals("VC2", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getThematicRole());
        assertEquals("開發", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(0).getChildAt(1).getData().getText());

        assertEquals("DE", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(1)
                .getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1)
                .getChildAt(1).getData().getThematicRole());
        assertEquals("的", result.getRoot().getChildAt(2).getChildAt(1).getChildAt(1).getChildAt(1)
                .getData().getText());
    }

    @Test
    public void testTaggedTextToTermTree5() {
        String sentence = "#1:1.[0] VP(Head:VC2:申辦|goal:NP(DUMMY1:NP("
                + "property:Nca:台灣|property:Nab:大哥大|Head:Nab:門號)|"
                + "Head:Caa:及|DUMMY2:NP(property:Nv1:促銷|Head:Nac:專案))|"
                + "complement:VP(deontics:Dbab:需|Head:VC32:攜帶|theme:NP("
                + "quantifier:Neqa:哪些|Head:Nab:證件)))#？(QUESTIONCATEGORY)";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);
        assertEquals("VP", result.getRoot().getData().getPos());
        assertEquals(3, result.getRoot().getChildren().size());

        assertEquals("VC2", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(0).getData().getThematicRole());
        assertEquals("申辦", result.getRoot().getChildAt(0).getData().getText());

        assertEquals("NP", result.getRoot().getChildAt(1).getChildAt(0).getData().getPos());
        assertEquals("DUMMY1", result.getRoot().getChildAt(1).getChildAt(0).getData()
                .getThematicRole());

        assertEquals("Nv1", result.getRoot().getChildAt(1).getChildAt(2).getChildAt(0)
                .getData().getPos());
        assertEquals("property", result.getRoot().getChildAt(1).getChildAt(2).getChildAt(0)
                .getData().getThematicRole());
    }

    @Test
    public void testTaggedTextToTermTree6() {
        String sentence = "#1:1.[0] S(theme:NP(property:GP‧的(head:GP("
                + "DUMMY:VP(reason:Dj:為什麼|Head:VA11:漫遊)|Head:Ng:時)|Head:DE:的)|"
                + "property:Nv:上網|Head:Nad:速度)|degree:Dfa:很|Head:VH13:慢)"
                + "#?(QUESTIONCATEGORY)";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);

        assertEquals("S", result.getRoot().getData().getPos());
        assertEquals(3, result.getRoot().getChildren().size());

        assertEquals("GP", result.getRoot().getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("property", result.getRoot().getChildAt(0).getChildAt(0).getData()
                .getThematicRole());

        assertEquals("VP", result.getRoot().getChildAt(0).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getPos());
        assertEquals("DUMMY", result.getRoot().getChildAt(0).getChildAt(0)
                .getChildAt(0).getChildAt(0).getData().getThematicRole());

        assertEquals("Ng", result.getRoot().getChildAt(0).getChildAt(0)
                .getChildAt(0).getChildAt(1).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(0).getChildAt(0)
                .getChildAt(0).getChildAt(1).getData().getThematicRole());
        assertEquals("時", result.getRoot().getChildAt(0).getChildAt(0)
                .getChildAt(0).getChildAt(1).getData().getText());
    }

    @Test
    public void testTaggedTextToTermTree7() {
        String sentence = "#2:1.[0] particle(Head:Ta:了)#";
        Tree<Term> result = ParserUtil.taggedTextToTermTree(sentence);
        assertNotNull(result);

        assertEquals("particle", result.getRoot().getData().getPos());
        assertEquals(1, result.getRoot().getChildren().size());

        assertEquals("Ta", result.getRoot().getChildAt(0).getData().getPos());
        assertEquals("Head", result.getRoot().getChildAt(0).getData().getThematicRole());
        assertEquals("了", result.getRoot().getChildAt(0).getData().getText());
    }
}
