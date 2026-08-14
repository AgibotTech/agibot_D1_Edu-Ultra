# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'D1_Edu-Ultra'
copyright = '2026, Agibot'
author = 'Agibot'
release = 'V0.2.7'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'myst_parser',
    'sphinx_tippy',
    'sphinx_multiversion',
    'sphinx_design',
    'sphinx_pdf_generate',
    'sphinxcontrib.mermaid',
]

templates_path = ['_templates']
exclude_patterns = []

language = 'zh_CN'

# 隐藏右上角 View Source 链接
html_copy_source = False
html_show_sourcelink = False

html_show_sphinx = False
html_secnumber_suffix = ' '

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
