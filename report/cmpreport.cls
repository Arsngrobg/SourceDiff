% Class declaration
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{cmpreport}[22/08/2020 v5.41 (Pierre Chardaire) Third year Project Reports - Modified by Arsngrobg]

% Base this class on KOMA script article class file
% A 5mm binding is added to margin
\LoadClass[12pt,BCOR=0.5cm,twoside=false]{scrartcl}
%\KOMAoptions{appendixprefix=true}.
% Font combination
\RequirePackage{mathptmx}                 %doc 3 format
\RequirePackage[scaled=0.9]{helvet}       %doc 3 format
\RequirePackage{courier}                  %doc 3 format
\RequirePackage[T1]{fontenc}
% needed for changing geometry of marksheets
\RequirePackage{geometry}
% for citing (and necessary for APA style)
\RequirePackage{natbib}
\bibliographystyle{apalike}
%
% Students will have figures in their report
%
\RequirePackage{graphicx}
%
% Ensure that table captions are above tables and figure captions are below figure
% independently of the position of the caption command in the latex source
%
\RequirePackage{float}
%\floatstyle{komabelow}
%\restylefloat{figure}
%\setlength{\abovecaptionskip}{5pt plus 2pt minus 2pt}
%\floatstyle{komaabove}
%\restylefloat{table}
%\setlength{\belowcaptionskip}{5pt plus 2pt minus 2pt}
%
% KOMA-script package for headings
%
\RequirePackage{scrlayer-scrpage}
%
% Enables to handle url in citations
% Also allow clickable section/citations/refs in PDF
%
\RequirePackage{hyperref}
%
% Links are underlined, but these lines do not appear in printed PDF
%
\hypersetup{colorlinks=false,pdfborderstyle={/S/U/W 1}}
%
% url wrapping
%
\g@addto@macro{\UrlBreaks}{\UrlOrds}
%
% url style: more compact than default tt 
%
\def\url@ukostyle{%
\@ifundefined{selectfont}{\def\UrlFont{\normalfont\sffamily}}{\def\UrlFont{\normalfont\sffamily\slshape}}} 
\urlstyle{uko}
%
% Captions are left aligned
%
\RequirePackage[justification=justified,singlelinecheck=false]{caption}
%
% Numbering of figures and table
%
%\RequirePackage{chngcntr}
%\counterwithin{figure}{section}
%\counterwithin{table}{section}
%\counterwithin{equation}{section}
%
% Creation of better tables
%
\RequirePackage{array}
\RequirePackage{ragged2e}
\RequirePackage{booktabs}
\newcolumntype{L}[1]{>{\RaggedRight\hspace{0pt}}p{\dimexpr #1\linewidth-2\tabcolsep}}
\newcolumntype{R}[1]{>{\RaggedLeft\hspace{0pt}}p{\dimexpr #1\linewidth-2\tabcolsep}}
\newcolumntype{C}[1]{>{\Centering\hspace{0pt}}p{\dimexpr #1\linewidth-2\tabcolsep}}
\newcolumntype{M}[1]{>{\Centering\hspace{0pt}}m{\dimexpr #1\linewidth-2\tabcolsep}}
%
\newlength{\arrayrulewidthOriginal}
\newcommand{\Hline}[1]{%
  \noalign{\global\setlength{\arrayrulewidthOriginal}{\arrayrulewidth}}%
  \noalign{\global\setlength{\arrayrulewidth}{#1 pt}}\hline%
  \noalign{\global\setlength{\arrayrulewidth}{\arrayrulewidthOriginal}}}
%
\newcommand{\LL}{\RaggedRight}
\newcommand{\RR}{\RaggedLeft}
\newcommand{\CC}{\Centering}
%
%
% As we don't want students to change the cmidrule weight we can make the option format 
% more consistent
% 
\RequirePackage{xparse}
%
\let\@originalcmidrule\cmidrule
\DeclareExpandableDocumentCommand{\cmidrule}{o m}{%
\IfNoValueTF{#1} 
       {\@originalcmidrule{#2}}
       {\@originalcmidrule (#1){#2}}
}
\renewcommand{\lightrulewidth}{\cmidrulewidth}
\newcommand{\uhrule}{\midrule[\heavyrulewidth]}
%
% To align numbers on decimal points in tables
% amongst other things
%
\RequirePackage{siunitx}
%
% To simplify use of multicolumns
%
\DeclareExpandableDocumentCommand{\mcc}{o d() m}{% 
\IfNoValueTF{#1} 
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{1}{c}{#3}}
        {\multicolumn{1}{C{#2}}{#3}}
    }
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{#1}{c}{#3}}
        {\multicolumn{#1}{C{#2}}{#3}}        
    }
}
\DeclareExpandableDocumentCommand{\mcl}{o d() m}{% 
\IfNoValueTF{#1} 
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{1}{l}{#3}}
        {\multicolumn{1}{L{#2}}{#3}}
    }
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{#1}{l}{#3}}
        {\multicolumn{#1}{L{#2}}{#3}}        
    }
}
\DeclareExpandableDocumentCommand{\mcr}{o d() m}{% 
\IfNoValueTF{#1} 
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{1}{r}{#3}}
        {\multicolumn{1}{R{#2}}{#3}}
    }
    {%    
        \IfNoValueTF{#2}
        {\multicolumn{#1}{r}{#3}}
        {\multicolumn{#1}{R{#2}}{#3}}        
    }
}
%
%
% table with tablenotes
%
\RequirePackage{threeparttable}
%
% to create note in middle of text
%
\newcommand{\mtnote}[1]{\textsuperscript{\TPTtagStyle{#1}}}
\g@addto@macro\TPT@opt@flushleft{\labelsep.0em}
%
% table environment
%
\let\@originaltable\table
\let\@endoriginaltable\endtable
\DeclareDocumentEnvironment{cmptable}{o m}
{\IfNoValueTF{#1} 
       {\@originaltable\threeparttable\caption{#2}}
       {\@originaltable[#1]\threeparttable\caption{#2}}}
{\endthreeparttable\@endoriginaltable}
%
\renewcommand{\TPTminimum}{\textwidth}
\newcommand*{\note}[1]{\item[\hspace{-2em}]#1}
%
\let\@originaltablenotes\tablenotes
\let\@endoriginaltablenotes\endtablenotes
\DeclareDocumentEnvironment{cmptablenotes}{}
{\footnotesize\@originaltablenotes[flushleft]}
{\@endoriginaltablenotes}
%
% For creating figures with subfigures
%
\RequirePackage{subcaption}
%
% Figure environment
%
\let\@originalfigure\figure
\let\@endoriginalfigure\endfigure
\DeclareDocumentEnvironment{cmpfigure}{o m}
{  \IfNoValueTF{#1} 
       {\@originalfigure}
       {\@originalfigure[#1]}}
{\caption{#2}\@endoriginalfigure}
%
% To create conditional lists of tables and list of figures
%
\RequirePackage{etoolbox}
\AtEndEnvironment{cmpfigure}{\gdef\there@is@a@figure{}} 
\AtEndEnvironment{cmptable}{\gdef\there@is@a@table{}} 
\AtEndEnvironment{figure}{\gdef\there@is@a@figure{}} 
\AtEndEnvironment{table}{\gdef\there@is@a@table{}} 
\AtEndDocument{\ifdefined\there@is@a@figure\label{fig:was:used:in:doc}\fi} 
\AtEndDocument{\ifdefined\there@is@a@table\label{tab:was:used:in:doc}\fi} 
%
\newcommand{\conditionalLoF}{\@ifundefined{r@fig:was:used:in:doc}{}{\listoffigures}}%
\newcommand{\conditionalLoT}{\@ifundefined{r@tab:was:used:in:doc}{}{\listoftables}}%
%
\newif\if@list
\@listtrue
\newcommand{\nolist}{\renewcommand{\listoffigures}{}\renewcommand{\listoftables}{}\@listfalse}
%
% For list of figures and list of tables on single page
%
\newif\if@twoPageLists
\@twoPageListstrue
\newcommand{\onePageLists}{\@twoPageListsfalse}
%
% For table of content
%
\RequirePackage[title,titletoc,toc]{appendix}
\RequirePackage[nottoc,notlof,notlot]{tocbibind}
\settocbibname{References}
%
% For mathematics
%
\RequirePackage{amsmath}
%
% for the admnin option (i.e. to write tutorials in same format as report)
%
\let\@originalauthor\author
\global\let\@myauthor\@empty
\renewcommand*{\author}[1]{%
\@originalauthor{#1}\renewcommand*{\@myauthor}{#1}}
\def\myname{\@myauthor}
%
% supervisor's name
%
\newcommand*{\supervisor}[1]{\renewcommand*{\@supervisor}{#1}}
\newcommand*{\@supervisor}{\ClassError{cmpfinalreport}{No \string\supervisor\space given}{}}
\def\mysupervisor{\@supervisor}
%
% registration number 
%
\newcommand*{\registration}[1]{\renewcommand*{\@registration}{#1}}
\newcommand*{\@registration}{\ClassError{cmpfinalreport}{No \string\registration\space given}{}}
\def\myreg{\@registration}
%
% course code
%
\newcommand*{\ccode}[1]{\renewcommand*{\@ccode}{#1}}
\newcommand*{\@ccode}{\ClassError{cmpfinalreport}{No \string\ccode\space given}{}}
%
% acknowledgements
% (used in final report only if students wants to include acknowledgements)
%
\global\let\@acknowledgements\@empty
\newcommand{\acknowledgements}[1]{\renewcommand{\@acknowledgements}{#1}}
%
% summary (replace use of abstract environment)
%
\newcommand{\summary}[1]{\renewcommand{\@summary}{#1}}
\newcommand{\@summary}{\ClassError{cmpfinalreport}{No \string\summary\space given}{}}
%
% preface for tutorial, e.g. used for handbook
%
\global\let\@preface\@empty
\newcommand{\preface}[1]{\renewcommand{\@preface}{#1}}
%
% watermark for confidential reports
%
\global\let\@confstatement\@empty
\newcommand{\confidential}[1]{
 \def\@confstatement{#1}
 \RequirePackage{eso-pic}
 \AddToShipoutPictureFG{
   \put(0,0){%
     \parbox[b][\paperheight]{\paperwidth}{%
       \vfill
       % free clipart image from https://www.clker.com/clipart-624476.html
       \includegraphics[width=0.3\paperwidth,height=0.3\paperheight,%
                        keepaspectratio]{confidentialWatermark.png}%
       \vfill
     }}}}
%
% to create a Gantt chart
%
\RequirePackage{pgfgantt}
%
% to pass to marksheet
%
\def\mytitle{\@title}
\def\meccode{\@ccode}
%
% Options. cmpreport can be used with four exclusive (exclusivity not tested) options
% proposal/review/progress/final for the four pieces of work returned by students
%
\newif\if@nottutorial
\@nottutorialtrue
\DeclareOption{tutorial}{
    \@nottutorialfalse
    \newcommand{\headingcmp}[1]{\renewcommand{\@headingcmp}{#1}}
    \newcommand{\@headingcmp}{Third year project tutorial}
}
\newif\if@cmptitlepage
\@cmptitlepagefalse
\DeclareOption{cmptitlepage}{
	\@cmptitlepagetrue
}
\newif\if@intermediate
\@intermediatetrue
\DeclareOption{final}{
    \@intermediatefalse
}
\newif\if@notprogress
\@notprogresstrue
\DeclareOption{progress}{
	\@notprogressfalse
	\newcommand*{\@worktitle}{Progress report}
	\AtBeginDocument{
		\let\default@color\current@color
		%\thispagestyle{empty}
		%\newgeometry{left=1cm,right=1cm,top=1cm,bottom=1cm}
		%\noindent\textbf{\@ccode}
		%\hspace{\stretch{1}}
		%\textbf{Student registration number: \@registration}
		%\\
		%\\
		%\InputIfFileExists{marksheetprogress.tex}{}{\ClassError{cmpfinalreport}{marksheetprogress.tex missing}{}}
		%\restoregeometry
		%\clearpage    
	    \setcounter{page}{1}
	    \maketitle
	    \let\maketitle\relax
	} %\AtBeginDocument
}
% Proposal no longer used since Oct 2024
%\DeclareOption{proposal}{
%    \newcommand*{\@worktitle}{Project proposal}
%    \AtEndDocument{
%        \clearpage
%        \thispagestyle{empty}
%        \newgeometry{left=1cm,right=1cm,top=1cm,bottom=1cm}
%        \renewcommand\thepage{}
%        \noindent{\textbf{\@ccode}}\hspace{\stretch{1}}
%        \textbf{Student registration number:\@registration}
%        \\
%        \\        
%        \InputIfFileExists{marksheetproposal.tex}{}{Missing marksheet}
%        \restoregeometry
%    } 
%} 
\DeclareOption{review}
{
    \newcommand*{\@worktitle}{Literature review}
    \AtEndDocument{
        \clearpage
        \thispagestyle{empty}
        \newgeometry{left=1cm,right=1cm,top=1cm,bottom=1cm}
        \renewcommand\thepage{}
        \textbf{\noindent\@ccode}
        \hspace{\stretch{1}}
        \textbf{Student registration number: \@registration}
        \\
        \\
        \InputIfFileExists{marksheet.tex}{}{Missing marksheet}
        \restoregeometry       
    } %\AtEndDocument
} %\DeclareOption{review}
\ProcessOptions
%
\if@notprogress
%\AddToHook{begindocument/end}{%
\AfterEndPreamble{%
    \let\default@color\current@color
    \maketitle
    \let\maketitle\relax
}
\else
%\IfFileExists{marksheetprogress.tex}{\RequirePackage[-22,nonofiles]{pagesel}}
%{
%{\Large Where is your marksheet \emph{marksheetprogress.tex}?}
%\RequirePackage[-1,nonofiles]{pagesel}}
\RequirePackage[-21,nonofiles]{pagesel}
\renewcommand{\baselinestretch}{1.2} 
\fi
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% headings
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\if@nottutorial
  %
  \newpagestyle{reportheadings}
  {(0pt,0pt){}{}{\@ccode\hfill\headmark}(\textwidth,1pt)} % header
  {(\textwidth,1pt){}{}{Reg:~\@registration\hfill\pagemark}(0pt,0pt)} % footer
  \newpagestyle{titlepageheading}
  {(0pt,0pt){}{}{\@ccode\hfill\@worktitle}(\textwidth,1pt)} % header
  {(\textwidth,1pt){}{}{Reg:~\@registration\hfill{}For\space\@supervisor}(0pt,0pt)} % footer
\else
  \newpagestyle{reportheadings}
  {(0pt,0pt){}{}{\@headingcmp\hfill\headmark}(\textwidth,1pt)} % header
  {(\textwidth,1pt){}{}{\@myauthor\hfill\pagemark}(0pt,0pt)} % footer
\fi
%
% Because \maketitle issue a \thisplagestyle{plain} at
% the end of execution
%
\g@addto@macro{\maketitle}{\thispagestyle{reportheadings}}
\if@nottutorial
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Inital proposal/literrature survey
  % (the name of supervisor appears in te title page headings)
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    \if@intermediate
    \renewcommand{\@maketitle}{%
        \thispagestyle{titlepageheading}
        \begin{center}
                \vspace*{\fill}
                \let\footnote\thanks
                {\LARGE\bfseries \@title\par}%
                \vskip 1em\relax
                \ifx\@myauthor\@empty
                    \relax
                \else
                    {\Large  \@myauthor}\\[\baselineskip]
                \fi
                {\large registration:~\@registration}\par
                \vskip 2\baselineskip\relax
                \ifx\@confstatement\@empty
                    \relax
                \else
                    \@confstatement\par
                \fi
                \vspace*{\fill}
        \end{center}
        \newpage
    }
    \pagestyle{reportheadings}
    \else
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Final report
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   
    \RequirePackage[-63,nonofiles]{pagesel}
    \newcounter{oldpage}
    \renewcommand{\@maketitle}{%
    	
    	\thispagestyle{empty}
    	%\newgeometry{left=1cm,right=1cm,top=1cm,bottom=1cm}
    	%\InputIfFileExists{marksheetsfinal.tex}{}{Missing marksheet}
    	%\restoregeometry
    	%\clearpage    
    	\setcounter{page}{1}    	
        \pagenumbering{roman}
        \thispagestyle{empty}     
        \newgeometry{left=2.5cm,right=2cm,top=1cm,bottom=2cm}
        { \centering
          \vspace*{\baselineskip}
          \ifx\@myauthor\@empty
              \relax
          \else
              {\Large  \@myauthor}\\[\baselineskip]
          \fi
          { Registration number~\@registration}\\[\baselineskip]
          {\Large \the\year}\\[\baselineskip]
          \rule{\textwidth}{1.6pt}\vspace*{-\baselineskip}\vspace{4pt}
          \rule{\textwidth}{0.4pt}\\
          \begingroup
             \hyphenpenalty 10000
             \exhyphenpenalty 10000
             {\noindent\Huge\bfseries \@title\par}
          \endgroup
          \rule{\textwidth}{0.4pt}\vspace*{-\baselineskip}\vspace{5pt}
          \rule{\textwidth}{1.6pt}\\
          \vskip 2\baselineskip\relax
          { Supervised by~\@supervisor}\par
          \vskip 2\baselineskip\relax
          \ifx\@confstatement\@empty
              \relax
          \else
              \@confstatement\par
          \fi
          \vspace*{\fill}
          \includegraphics[width=3cm]{uealogo.png}\\
          { University of East Anglia}\\
          { Faculty of Science}\\
          { School of Computing Sciences}\\
          \par
        }
        \restoregeometry
        \newpage
        \thispagestyle{empty}
        %
        %  Dealing with optional acknowledgements
        %
        
        \section*{Abstract}
        \@summary
        \ifx\@acknowledgements\@empty
                \relax
        \else
                \section*{Acknowledgements}
                \@acknowledgements
        \fi %
        \newpage
        \tableofcontents
        \newpage
        \if@list
           \conditionalLoF
           \if@twoPageLists 
              \newpage
           \fi
           \conditionalLoT
           \newpage
        \fi
        \setcounter{oldpage}{\value{page}}
        \pagenumbering{arabic}
        \setcounter{page}{\value{oldpage}}
      }
      \pagestyle{reportheadings}
      %\pagestyle{plain}
      \renewcommand{\baselinestretch}{1.2}
\fi
%
%
%
\else
\if@cmptitlepage
    \newcounter{oldpage}
    \headingcmp{Third year project handbook}    
    \renewcommand{\@maketitle}{%
    	\pagenumbering{roman}
    	\thispagestyle{empty}     
    	\newgeometry{left=2.5cm,right=2cm,top=1cm,bottom=2cm}
    	{ \centering
    		\vspace*{\baselineskip}
    		\ifx\@myauthor\@empty
    		\relax
    		\else
    		{\Large  \@myauthor}\\[\baselineskip]
    		\fi
    		{\Large Version \@ccode}\\[\baselineskip]
    		\rule{\textwidth}{1.6pt}\vspace*{-\baselineskip}\vspace{4pt}
    		\rule{\textwidth}{0.4pt}\\
    		\begingroup
    		\hyphenpenalty 10000
    		\exhyphenpenalty 10000
    		{\noindent\Huge\bfseries \@title\par}
    		\endgroup
    		\rule{\textwidth}{0.4pt}\vspace*{-\baselineskip}\vspace{5pt}
    		\rule{\textwidth}{1.6pt}\\
    		\vskip 2\baselineskip\relax
    		\vspace*{\fill}
    		\includegraphics[width=3cm]{uealogo.png}\\
    		{ University of East Anglia}\\
    		{ Faculty of Science}\\
    		{ School of Computing Sciences}\\
    		\par
    	}
    	\restoregeometry
    	\newpage
    	\thispagestyle{empty}
    	\ifx\@preface\@empty
    	   \relax
    	\else
    	   \section*{Preface}
    	   \@preface
           \newpage
        \fi
    	\thispagestyle{empty}
        \tableofcontents
        \newpage
        \if@list
           \conditionalLoF
           \if@twoPageLists 
              \newpage
           \fi
           \conditionalLoT
           \newpage
        \fi
        \setcounter{oldpage}{\value{page}}
        \pagenumbering{arabic}
        \setcounter{page}{\value{oldpage}}
    }

\fi
\pagestyle{reportheadings}
\fi %end \if@nottutorial 
