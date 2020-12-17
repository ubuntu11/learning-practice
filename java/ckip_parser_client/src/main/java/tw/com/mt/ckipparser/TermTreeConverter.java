/*
 * TermTreeConverter.java    1.0 2017年5月23日
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

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang3.StringUtils;

import com.c05mic.generictree.Node;
import com.c05mic.generictree.Tree;

/**
 * 把ckip parser產生的分析結果字串(ex: #1:1.[0] S(NP(N:手機|Head:Nv:上網)|ADV:如何|Head:Vt:設定)
 * #?(QUESTIONCATEGORY))轉換成Term的樹狀結構.
 *
 * @version 1.0 2017年5月23日
 * @author ken
 *
 */
public class TermTreeConverter {
    /**
     * ckip parser產生的分析結果字串.
     */
    private String text;
    /**
     * Default construictor.
     * @param text ckip parser產生的分析結果字串
     */
    public TermTreeConverter(String text) {
        super();
        this.text = text;
    }

    /**
     * 唯一的公開方法,將ckip parser的分析結果字串轉換成Term的樹狀結構.
     * @return Term的樹狀結構
     */
    public Tree<Term> convert() {
        Tree<Term> tree = null;
        Node<Term> root = null;
        Pattern p = Pattern.compile("([A-Za-z]+)(\\()(.*)(\\)#)");
        Matcher m = p.matcher(text);

        if (m.find()) {
            Term t = new Term();
            t.setPos(m.group(1));
            root = new Node<Term>(t);
            tree = new Tree<Term>(root);
            text = m.group(3);
        }

        String[] phrases = this.splitPhrases(text);
        for (String phrase : phrases) {
            handlePhrase(phrase, root);
        }
        return tree;
    }
    /**
     * 找出用來切分詞組的字元位置.
     * @return 字元位置數字陣列
     */
    private Integer[] findSeperatorPositions(String text) {
        List<Integer> postions = new ArrayList<Integer>();
        for (int i = 0; i < text.length(); i++) {
            if (text.charAt(i) == '|' && isSeperator(text, i)) {
                postions.add(i);
            }
        }
        return postions.toArray(new Integer[0]);
    }

    /**
     * 用vertical bar來切分詞組.
     * @return 詞組字元陣列
     */
    private String[] splitPhrases(String text) {
        int start = 0;
        Integer[] seperators = findSeperatorPositions(text);
        String[] result = new String[seperators.length + 1];
        for (int i = 0; i < result.length; i++) {
            if (i < seperators.length) {
                result[i] = StringUtils.substring(text, start, seperators[i]);
                start = seperators[i] + 1;
            } else {
                result[i] = StringUtils.substring(text, start);
            }
        }
        return result;
    }

    /**
     * 如果一個vertical bar左側的左括號數目與右括號數目相等,代表可用它來切分詞組.
     * @param text ckip parser的分析結果字串
     * @param pos vertical bar所在位置
     * @return true if 需要轉換
     */
    private boolean isSeperator(String text, int pos) {
        String leftHandSide = text.substring(0, pos);
        return StringUtils.countMatches(leftHandSide, "(") == StringUtils.countMatches(leftHandSide,
                ")");
    }

    /**
     * 處理詞組.
     * @param phrase 詞組
     * @param current 目前節點
     */
    private void handlePhrase(String phrase, Node<Term> current) {
        for (String subPhrase : splitPhrases(phrase)) {
            if (subPhrase.contains("(")) {
                handleComplexPhrase(subPhrase, current);
            } else {
                handleSimplePhrase(subPhrase, current);
            }
        }
    }

    /**
     * 處理複雜詞組,ex: NP(N:手機|Head:Nv:上網).
     * @param phrase 詞組
     * @param current 目前節點
     */
    private void handleComplexPhrase(String phrase, Node<Term> current) {
        //某些情況下會出現: "property:NP‧的"這樣的詞組，例如:
        Pattern p = Pattern.compile("([a-zA-Z0-9]*):?([A-Z]*)([•‧]?的?)(\\()(.*)(\\))");
        Matcher m = p.matcher(phrase);
        if (m.find()) {
            Term t = new Term();
            if (m.group(2).isEmpty()) {
                t.setPos(m.group(1));
            } else {
                t.setThematicRole(m.group(1));
                t.setPos(m.group(2));
            }
            Node<Term> nt = new Node<Term>(t);
            current.addChild(nt);
            phrase = m.group(5);
            handlePhrase(phrase, nt);
        }
    }

    /**
     * 處理簡單詞組,ex: ADV:如何.
     * @param phrase 詞組
     * @param current 目前節點
     */
    private void handleSimplePhrase(String phrase, Node<Term> current) {
        //simple phrase should not contain ")"
        String[] terms = StringUtils.split(StringUtils.remove(phrase, ")"), "|");
        for (String term : terms) {
            String[] data = StringUtils.split(term, ':');
            Term t = new Term();
            if (data.length == 2) {
                t.setPos(data[0]);
                t.setText(data[1]);
            } else {
                t.setThematicRole(data[0]);
                t.setPos(data[1]);
                t.setText(data[2]);
            }
            current.addChild(new Node<Term>(t));
        }
    }
}
