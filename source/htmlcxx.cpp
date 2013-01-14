#include "html/ParserDom.h"
#include "html/utils.h"
#include "html/wincstring.h"
#include "css/parser_pp.h"

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <cstdio>

#include "liste.h"
#include "main.h"

#define VERSION "0.6"

extern "C" {
#include "entities.h"
}

using namespace std;
using namespace htmlcxx;

enum {title,a,form,text,all};

void save_mem(string str)
{
    ofstream file;
    file.open ("debug.txt", ios::out | ios::app);
    file << str << endl << endl;
    file.close();
}

void save_mem_int(float str)
{
    ofstream file;
    file.open ("debug.txt", ios::out | ios::app);
    file << str << endl << endl;
    file.close();
}

int checkparent(vector<string> tag, int start)
{
    unsigned int i, k;
    for (i=0; i<tag.size(); i++)
    {
        for (k=start; k<END; k++)
        {
            if (!stricmp(tag[i].c_str(), tags[k]))
            {
                return 1;
            }
        }
    }
    return 0;
}

int checkTag(vector<string> tag, string name)
{
    unsigned int i;
    for (i=0; i<tag.size(); i++)
    {
        if (!stricmp(tag[i].c_str(), name.c_str()))
            return 1;
    }
    return 0;
}

int checkchr (string value)
{
    unsigned int i;
    for (i=0; i<value.length(); i++)
        if (value[i]>32)
            return 1;
    return 0;
}

int isContainer(string name)
{
    string array[] = {"h1","h2","h3","h4","h5","h6","p","div","br","center","hr","li","title","form","img"};
    vector<string> container (array, array + sizeof(array) / sizeof(string));
    return checkTag(container, name);
}

string findText(tree<HTML::Node>::iterator it)
{
    tree<HTML::Node>::iterator begin, ends;
    string text;
    begin = it;
    ends = it;
    ends.skip_children();
    ++ends;
    for (; begin != ends; ++begin)
    {
        if (!begin->isTag() && !begin->isClosing() && !begin->isComment())
        {
            char *decode=(char*)malloc(begin->text().length()+1);
            decode_html_entities_utf8(strcpy(decode, begin->text().c_str()),NULL);
            text.append(decode);
            free(decode);
        }
    }
    return text;
}

string search(string id, tree<HTML::Node>::iterator it)
{
    tree<HTML::Node>::iterator begin, ends;
    begin = it;
    ends = it;
    ends.skip_children();
    ++ends;
    for (; begin != ends; ++begin)
    {
        if (begin->tagName()=="label")
        {
            begin->parseAttributes();
            if(begin->attribute("for").second==id)
                return findText(begin);
        }
    }
    return "noLabel";
}

tree<HTML::Node>::iterator thtml(tree<HTML::Node>::iterator it, tree<HTML::Node>::iterator end, string dest)
{
    tree<HTML::Node>::iterator k = it;
    for (; it!=end; it++)
    {
        if (!it->isTag())
            continue;
        if (!strcasecmp(it->tagName().c_str(), dest.c_str()))
            return it;
    }
    return k;
}

typedef struct
{
    bool open;
    Tag* p;
} last;

Lista getTag(char * buffer)
{
    bool parse_css=true;
    string css_code;
    tree<HTML::Node> dom;
    Lista l1;

    try
    {
        string html(buffer);
        HTML::ParserDom parser;
        CSS::Parser css_parser;
        char *decode=NULL;

        //Dump all text of the document
        dom = parser.parseTree(html);
        tree<HTML::Node>::iterator it = dom.begin();
        tree<HTML::Node>::iterator end = dom.end();

        vector<string> parent;
        last open[all]= { {false},{false},{false},{false} };
        it=thtml(it,end,"html");
        Value out;

        if(css_code.length())
        {
            css_parser.parse(css_code);
        }

        for (; it!=end; ++it)
        {
            it->parseAttributes();
            if (it->tagName()=="script" ||
                    it->attribute("style").second.find("display:none")!=string::npos)
            {
                it.skip_children();
                continue;
            }
            if (!it->isTag() && !it->isClosing() && !it->isComment())
            {
                if (!parent.empty() && checkparent(parent,TITLE))
                {
                    decode=(char*)malloc(it->text().length()+1);
                    decode_html_entities_utf8(strcpy(decode, it->text().c_str()),NULL);
                    out.text.assign(decode);
                    free(decode);
                    out.mode=parent;
                    if (open[title].open)
                        open[title].p->value.push_back(out);
                    else if (open[a].open)
                        open[a].p->value.push_back(out);
                    else if (open[text].open)
                    {
                        if (checkchr(out.text))
                            open[text].p->value.push_back(out);
                    }
                    else
                    {
                        if (checkchr(out.text))
                        {
                            l1.push_back({"text"});
                            open[text].open=true;
                            open[text].p=&(*l1.rbegin());
                            l1.rbegin()->value.push_back(out);
                        }
                    }
                }
            }

            else if (it->isTag())
            {
                if (parse_css)
                {
                    vector<CSS::Parser::Selector> v;
                    tree<HTML::Node>::iterator k = it;
                    while (k != dom.begin())
                    {
                        CSS::Parser::Selector s;
                        s.setElement(k->tagName());
                        s.setId(k->attribute("id").second);
                        s.setClass(k->attribute("class").second);
                        s.setPseudoClass(CSS::Parser::NONE_CLASS);
                        s.setPseudoElement(CSS::Parser::NONE_ELEMENT);
                        v.push_back(s);
                        k = dom.parent(k);
                    }

                    map<string, string> attributes = css_parser.getAttributes(v);
                    map<string, string>::const_iterator mit = attributes.begin();
                    map<string, string>::const_iterator mend = attributes.end();

                    bool skip=false;
                    for (; mit != mend; ++mit)
                    {
                        if (mit->first=="display" && mit->second=="none")
                            skip=true;
                    }

                    if (strcasecmp(it->tagName().c_str(), "STYLE") == 0)
                    {
                        tree<HTML::Node>::iterator begin, end;
                        begin = it;
                        end = it;
                        end.skip_children();
                        ++end;
                        string css_snippet;

                        for (; begin != end; ++begin)
                        {
                            if (!(begin->isTag()))
                                css_snippet.append(begin->text());
                        }
                        css_parser.parse(css_snippet);
                        skip=true;
                    }

                    if (skip && !open[form].open)
                    {
                        it.skip_children();
                        continue;
                    }
                }

                if (it->closingText().length()>0)
                {
                    string tag=it->tagName();
                    parent.push_back(tag);
                }
                if (isContainer(it->tagName()))
                {
                    if (it->tagName()=="p")
                        l1.push_back({"p"});
                    else l1.push_back({"return"});
                    open[text].open=false;
                }
                if (it->tagName() == "title")
                {
                    l1.push_back({"title"});
                    open[title].open=true;
                    open[title].p=&(*l1.rbegin());
                }
                else if (it->tagName() == "a")
                {
                    l1.push_back({"a"});
                    l1.rbegin()->attribute.append (it->attribute("href").second);
                    open[a].open=true;
                    open[a].p=&(*l1.rbegin());
                    open[text].open=false;
                }
                else if (it->tagName() == "img")
                {
                    l1.push_back({"img"});
                    l1.rbegin()->value.push_back ({it->attribute("src").second});
                    l1.rbegin()->attribute.append (it->attribute("height").second);
                }
                else if (it->tagName() == "form")
                {
                    l1.push_back({"form"});
                    l1.rbegin()->form.action.append (it->attribute("action").second);
                    l1.rbegin()->form.method.append (it->attribute("method").second);
                    open[form].open=true;
                    open[form].p=&(*l1.rbegin());
                }
                else if (it->tagName() == "input")
                {
                    if (open[form].open)
                    {
                        tree<HTML::Node>::iterator k = it;
                        while (k->tagName() != "form" && k != dom.begin())
                            k = dom.parent(k);
                        string label=search(it->attribute("id").second, k);
                        open[form].p->form.input=InsInFondo(open[form].p->form.input, it->attribute("name").second, it->attribute("type").second, it->attribute("value").second, label);
                    }
                }
                else if (it->tagName() == "meta")
                {
                    l1.push_back({"meta"});
                    l1.rbegin()->value.push_back ({it->attribute("http-equiv").second});
                    l1.rbegin()->attribute.append (it->attribute("content").second);
                }
            }

            else if (!it->isComment())
            {
                if (!parent.empty())
                {
                    parent.pop_back();
                    if (it->tagName()=="a")
                    {
                        if (open[a].p->value.size()==0)
                            open[a].p->value.push_back ({"LINK"});
                        open[a].open=false;
                    }
                    else if (it->tagName()=="title")
                        open[title].open=false;
                    else if (it->tagName()=="form")
                        open[form].open=false;
                }
                if (isContainer(it->tagName()))
                {
                    if (it->tagName()=="p")
                        l1.push_back({"p"});
                    else l1.push_back({"return"});
                    open[text].open=false;
                }
                else if (it->tagName()=="a")
                    open[text].open=false;
            }
        }
    }

    catch (exception &e)
    {
        cerr << "Exception " << e.what() << " caught" << endl;
        exit(1);
    }
    catch (...)
    {
        cerr << "Unknow exception caught " << endl;
    }
    return l1;
}
