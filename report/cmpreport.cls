\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{ueacmpreport}[03-10-2025 v5.42 (Pierre Chardaire) Third year Project Reports - Modified by Arsngrobg]

% Base this class on KOMA script article class file - A 5mm binding is added to margin
\LoadClass[12pt, BCOR=0.5cm, twoside=false]{scrartcl}

\newlength{\arrayrulewidthOriginal}

% Global Packages:
    % Font Combinations:
    \RequirePackage{mathptmx}
    \RequirePackage[scaled=0.9]{helvet}
    \RequirePackage{courier}
    \RequirePackage[T1]{fontenc}

    % Changing Geometry of Marksheets
    \RequirePackage{geometry}

    % Citing Resources in APA Style
    \RequirePackage{natbib}
    \bibliographystyle{apalike}

    % Figures
    \RequirePackage{graphicx}

    % Headings
    \RequirePackage{scrlayer-scrpage}

    % Table Are Above Tables and Figure Captions are Below Figure Captions - Independent  of position of caption command
    \RequirePackage{float}

    % Enables Clickable Section/Citations/Refs in PDF
    \RequirePackage{hyperref}

    % Underlined Links in PDF
    \hypersetup{colorlinks=false, pdfborderstyle={/S/U/W 1}}

    % Left-Aligned Captions
    \RequirePackage[justification=justified, singlelinecheck=false]{caption}

    % Creation of Better Tables
    \RequirePackage{array}
    \RequirePackage{ragged2e}
    \RequirePackage{booktabs}

    % Consistent Format of cmidrule Weight
    \RequirePackage{xparse}

    % Alignment of Numbers on Decimal Points in Tables and More
    \RequirePackage{siunitx}

    % Creating Figures with Subfigures
    \RequirePackage{subcaption}

    % Table notes
    \RequirePackage{threeparttable}

    % To create conditional lists of tables and list of figures
    \RequirePackage{etoolbox}

    % Table of Contents
    \RequirePackage[title, titletoc, toc]{appendix}
    \RequirePackage[nottoc, notlof, notlot]{tocbibind}

    % Mathematics
    \RequirePackage{amsmath}

    % Gantt Chart
    \RequirePackage{pgfgantt}

% Global Commands:
    % \mytitle                 -> Gets the title of the document
    \newcommand\mytitle{ \@title }

    % \author                  -> Defines the author for this document and a member type
    % \myauthor                -> Gets the author of this document
    \newcommand*{\@myauthor}{}
    \renewcommand*{\author}[1] {
        \renewcommand*{\@myauthor}{ #1 }
    }
    \newcommand*{\myname}{ \@myauthor }

    % \supervisor{name}        -> Assigns a supervisor to this document
    % \mysupervisor            -> Gets the assigned supervisor
    \newcommand*{\supervisor}[1] {
        \renewcommand*{\@supervisor}{ #1 }
    }
    \newcommand*{\@supervisor} {
        \ClassError{cmpfinalreport}{ No \string\supervisor\space given }{}
    }
    \newcommand{\mysupervisor}{ \@supervisor }

    % \registration{id}        -> Assigns a UEA student registration number to this document
    % \myreg                   -> Gets the student's UEA registration number
    \newcommand*{\registration}[1] { \renewcommand*{\@registration}{#1} }
    \newcommand*{\@registration}{ \ClassError{cmpfinalreport}{No \string\registration\space given}{} }
    \newcommand{\myreg}{ \@registration }

    % \ccode{code}             -> Assigns the 3rd year project module code to this document
    % \mycode                  -> Gets the 3rd year project module code
    \newcommand*{\ccode}[1] { \renewcommand*{\@ccode}{#1} }
    \newcommand*{\@ccode} { \ClassError{cmpfinalreport}{No \string\ccode\space given}{} }
    \newcommand{\myccode} { \@ccode }

    % \acknowledgements{ack}   -> Used in the final report to include any acknowledgements
    \newcommand\@acknowledgements\@empty
    \newcommand{\acknowledgements}[1]{ \renewcommand{\@acknowledgements}{#1} }

    % \summary{sum}            -> Replaces use of abstract environment and acts as a summarising text for the document
    \newcommand{\summary}[1] { \renewcommand{\@summary}{#1} }
    \newcommand{\@summary}{ \ClassError{cmpfinalreport}{No \string\summary\space given}{} }

    % \confstatement{statement} -> Provides a censoring watermark for confidential statements
    \global\newcommand{\@confstatement}\empty
    \newcommand{\confidential}[1] {
        \renewcommand\@confstatement{#1}
        \RequirePackage{eso-pic}
        \AddToShipoutPictureFG {
            \put(0, 0) {
                \parbox[b][\paperheight]{\paperwidth} {
                    \vfill
                    \includegraphics[width=0.3\paperwidth, height=0.3\paperheight, keepaspectratio]
                    {img/confidential}
                    \vfill
                }
            }
        }
    }

    % \nolist                  -> Sets the document to not use Table of Figures & Table of Contents
    \newif\if@list
    \@listtrue
    \newcommand{\nolist} {
        \renewcommand{\listoffigures}\empty
        \renewcommand{\listoftables}\empty
        \@listfalse
    }

    % \twoPageLists             -> Formats the Lists of Tables & List of Figures
    \newif\if@twoPageLists
    \@twoPageListstrue
    \newcommand{\onePageLists}{\@twoPageListsfalse}

    % \HLine{thickness}        -> Displays a horizontal line of thickness
    \newcommand{\HLine}[1] {
        \noalign {
            \global\setlength
            {\arrayrulewidthOriginal}
            {\arrayrulewidth}
        }
        \noalign {
            \global\setlength
            {\arrayrulewidth}
            {#1 pt}
        }\hline
        \noalign {
            \global\setlength
            {\arrayrulewidth}
            {\arrayrulewidthOriginal}
        }
    }

    % \mtnote{content}         -> Displays a note in the middle of the page
    \newcommand{\mtnote}[1] { \textsuperscript{\TPTtagStyle{#1}} }

    % \hrule                   -> Shorthand for displaying a thick horizontal line in a table
    \newcommand{\uhrule} { \midrule[\heavyrulewidth] }

    % \LL                      -> Shorthand for \RaggedRight
    \newcommand{\LL} { \RaggedRight }

    % \RR                      -> Shorthand for \RaggedLeft
    \newcommand{\RR} { \RaggedLeft }

    % \CC                      -> Shorthand for \Centering
    \newcommand{\CC} { \Centering }

% Style Configurations:
    % URL Styling:
    {
        % URL wrapping
        \g@addto@macro{\UrlBreaks}{\UrlOrds}

        % URL style: more compact than default
        \newcommand{\url@ukostyle} {
            \@ifundefined{selectfont} {
                \renewcommand\UrlFont{\normalfont\sffamily}
            }{
                \renewcommand\UrlFont{\normalfont\sffamily\slshape}
            }
        }
        % Immediately apply the URL style across the report
        \urlstyle{uko}
    }

    % Better Tables:
    {
        \newcolumntype{L}[1]{>{\RaggedRight\hspace{0pt}}p{\dimexpr \#1\linewidth-2\tabcolsep}}
        \newcolumntype{R}[1]{>{\RaggedLeft\hspace{0pt}}p{\dimexpr \#1\linewidth-2\tabcolsep}}
        \newcolumntype{C}[1]{>{\Centering\hspace{0pt}}p{\dimexpr \#1\linewidth-2\tabcolsep}}
        \newcolumntype{M}[1]{>{\Centering\hspace{0pt}}m{\dimexpr \#1\linewidth-2\tabcolsep}}
    }

    % Consistent cmidrule Weight
    {
        \newcommand{\@originalcmidrule}{\cmidrule}
        \DeclareExpandableDocumentCommand{\cmidrule}{o m} {
            \IfNoValueTF{\#1}
            {\@originalcmidrule{\#2}}
            {\@originalcmidrule (\#1){\#2}}
        }
        \renewcommand{\lightrulewidth}{\cmidrulewidth}
    }

    % Multi-Columns
    {
        \DeclareExpandableDocumentCommand{\mcc}{o d() m} {
            \IfNoValueTF{\#1} {
                \IfNoValueTF{\#2}
                {\multicolumn{1}{c}{\#3}}
                {\multicolumn{1}{C{\#2}}{\#3}}
            } {
                \IfNoValueTF{\#2}
                {\multicolumn{\#1}{c}{\#3}}
                {\multicolumn{\#1}{C{\#2}}{\#3}}
            }
        }
        \DeclareExpandableDocumentCommand{\mcl}{o d() m} {
            \IfNoValueTF{\#1} {
                \IfNoValueTF{\#2}
                {\multicolumn{1}{l}{\#3}}
                {\multicolumn{1}{L{\#2}}{\#3}}
            }{
                \IfNoValueTF{\#2}
                {\multicolumn{\#1}{l}{\#3}}
                {\multicolumn{\#1}{L{\#2}}{\#3}}
            }
        }
        \DeclareExpandableDocumentCommand{\mcr}{o d() m}{
            \IfNoValueTF{\#1}
            {
                \IfNoValueTF{\#2}
                {\multicolumn{1}{r}{\#3}}
                {\multicolumn{1}{R{\#2}}{\#3}}
            }
            {
                \IfNoValueTF{\#2}
                {\multicolumn{\#1}{r}{\#3}}
                {\multicolumn{\#1}{R{\#2}}{\#3}}
            }
        }
    }

    % Zero Spacing in Flush Left Layout
    \g@addto@macro\TPT@opt@flushleft{\labelsep.0em}

    % Table Environments
    {
        \newcommand{\@originaltable}{\table}
        \newcommand{\@endoriginaltable}{\endtable}
        \DeclareDocumentEnvironment{cmptable}{o m}
        {\IfNoValueTF{#1}
        {\@originaltable\threeparttable\caption{#2}}
        {\@originaltable[#1]\threeparttable\caption{#2}}}
        {\endthreeparttable\@endoriginaltable}

        \renewcommand{\TPTminimum}{\textwidth}
        \newcommand*{\note}[1]{\item[\hspace{-2em}]#1}

        \newcommand{\@originaltablenotes}{\tablenotes}
        \newcommand{\@endoriginaltablenotes}{\endtablenotes}
        \DeclareDocumentEnvironment{cmptablenotes}{}
        {\footnotesize\@originaltablenotes[flushleft]}
        {\@endoriginaltablenotes}
    }

    % Figure Environments
    {
        \newcommand\@originalfigure\figure
        \newcommand\@endoriginalfigure\endfigure
        \DeclareDocumentEnvironment{cmpfigure}{o m} {
            \IfNoValueTF{#1} {
                {\@originalfigure}
                {\@originalfigure[#1]}
            }
        }
        {\caption{#2}\@endoriginalfigure}
    }

    % Optional Lists of Tables & Lists of Figures
    {
        \AtEndEnvironment{cmpfigure}{\gdef\there@is@a@figure{}}
        \AtEndEnvironment{cmptable}{\gdef\there@is@a@table{}}
        \AtEndEnvironment{figure}{\gdef\there@is@a@figure{}}
        \AtEndEnvironment{table}{\gdef\there@is@a@table{}}
        \AtEndDocument{\ifdefined\there@is@a@figure\label{fig:was:used:in:doc}\fi}
        \AtEndDocument{\ifdefined\there@is@a@table\label{tab:was:used:in:doc}\fi}

        \newcommand{\conditionalLoF}{\@ifundefined{r@fig:was:used:in:doc}{}{\listoffigures}}
        \newcommand{\conditionalLoT}{\@ifundefined{r@tab:was:used:in:doc}{}{\listoftables}}
    }

% Document Options [review, progress, final]:
    % Declaring conditionals to use the correct styling depending on the option:
    \newif\if@nottutorial
        \@nottutorialtrue
    \newif\if@cmptitlepage
        \@cmptitlepagefalse
    \newif\if@intermediate
        \@intermediatetrue
    \newif\if@notprogress
        \@notprogresstrue

    % Setting the conditions based on the option type:
    \DeclareOption{tutorial} {
        \@nottutorialfalse
        \newcommand{\headingcmp}[1] {
            \renewcommand{\@headingcmp} { #1 }
        }
        \newcommand{\@headingcmp} { Third year project tutorial }
    }

    \DeclareOption{cmptitlepage} {
        \@cmptitlepagetrue
    }

    \DeclareOption{final} {
        \@intermediatefalse
    }

    \DeclareOption{progress} {
        \@notprogressfalse
        \newcommand*{\@worktitle} { Progress report }
        \AtBeginDocument {
            \edef\default@color{\current@color}
            \setcounter{page}{1}
            \maketitle
            \renewcommand\maketitle\relax
        }
    }

    \DeclareOption{review} {
        \renewcommand*{\@worktitle}{Literature review}
        \AtEndDocument{
            \clearpage
            \thispagestyle{empty}
            \newgeometry{left=1cm, right=1cm, top=1cm, bottom=1cm}
            \renewcommand\thepage{}
            \textbf{\noindent\@ccode}
            \hspace{\stretch{1}}
            \textbf{Student registration number: \@registration}
            \\
            \\
            \InputIfFileExists{marksheetlitrev.tex}{}{Missing marksheet}
            \restoregeometry
        }
    }

    % Process the options
    \ProcessOptions

    % Apply styling:
    \if@notprogress
        \AfterEndPreamble {
            \edef\default@color{\current@color}
            \maketitle
            \renewcommand\maketitle\relax
        }
    \else
        \RequirePackage[-21, nonofiles]{pagesel}
        \renewcommand{\baselinestretch} { 1.2 }
    \fi

    \if@nottutorial
        \newpagestyle{reportheadings}
        {(0pt, 0pt){}{}{\@ccode\hfill\headmark}(\textwidth, 1pt)}                           % header
        {(\textwidth, 1pt){}{}{Reg:~\@registration\hfill\pagemark}(0pt,0pt)}                % footer
        \newpagestyle{titlepageheading}
        {(0pt, 0pt){}{}{\@ccode\hfill\@worktitle}(\textwidth, 1pt)}                         % header
        {(\textwidth, 1pt){}{}{Reg:~\@registration\hfill{}For\space\@supervisor}(0pt, 0pt)} % footer
    \else
        \newpagestyle{reportheadings}
        {(0pt, 0pt){}{}{\@headingcmp\hfill\headmark}(\textwidth, 1pt)}                      % header
        {(\textwidth, 1pt){}{}{\@myauthor\hfill\pagemark}(0pt, 0pt)}                        % footer
    \fi

    % Because \maketitle issue a \thisplagestyle{plain} at
    % the end of execution
    \g@addto@macro{\maketitle}{\thispagestyle{reportheadings}}
    \if@nottutorial
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Initial proposal/literature survey
    % (the name of supervisor appears in te title page headings)
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    \if@intermediate
        \renewcommand{\@maketitle} {
            \thispagestyle{titlepageheading}
            \begin{center}
                \vspace*{\fill}
                \renewcommand\footnote\thanks
                {\LARGE\bfseries \@title\par}
                \vskip 1em\relax
                \ifx\@myauthor\@empty
                    \relax
                \else
                    {\Large\@myauthor}\\[\baselineskip]
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
        % Final
        \RequirePackage[-63, nonofiles]{pagesel}
        \newcounter{oldpage}
        \renewcommand{\@maketitle} {
            \thispagestyle{empty}
            \setcounter{page}{1}
            \pagenumbering{roman}
            \thispagestyle{empty}
            \newgeometry{left=2.5cm, right=2cm, top=1cm, bottom=2cm} {
                \centering
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
                \includegraphics[width=3cm]{img/uealogo}\\
                { University of East Anglia}\\
                { Faculty of Science}\\
                { School of Computing Sciences}\\
            }
            \restoregeometry
            \newpage
            \thispagestyle{empty}

            % Dealing with optional acknowledgements
            \section*{Abstract}
            \@summary
            \ifx\@acknowledgements\@empty
                \relax
            \else
                \section*{Acknowledgements}
                \@acknowledgements
            \fi
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

        \renewcommand{\baselinestretch} { 1.2 }
    \fi

    \else
        \if@cmptitlepage
        \newcounter{oldpage}
        \headingcmp{Third year project handbook}
        \renewcommand{\@maketitle}{
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
                \includegraphics[width=3cm]{img/uealogo}\\
                { University of East Anglia}\\
                { Faculty of Science}\\
                { School of Computing Sciences}\\
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
\fi
