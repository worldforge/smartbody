<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en-US">
<head>
<link rel="icon" href="/cpython/static/hgicon.png" type="image/png" />
<meta name="robots" content="index, nofollow" />
<link rel="stylesheet" href="/cpython/static/style-paper.css" type="text/css" />
<script type="text/javascript" src="/cpython/static/mercurial.js"></script>

<link rel="stylesheet" href="/cpython/highlightcss" type="text/css" />
<title>cpython: 0dbdaa0ff648 Lib/random.py</title>
</head>
<body>

<div class="container">
<div class="menu">
<div class="logo">
<a href="http://hg.python.org/">
<img src="/cpython/static/hglogo.png" alt="back to hg.python.org repositories" /></a>
</div>
<ul>
<li><a href="/cpython/shortlog/0dbdaa0ff648">log</a></li>
<li><a href="/cpython/graph/0dbdaa0ff648">graph</a></li>
<li><a href="/cpython/tags">tags</a></li>
<li><a href="/cpython/branches">branches</a></li>
</ul>
<ul>
<li><a href="/cpython/rev/0dbdaa0ff648">changeset</a></li>
<li><a href="/cpython/file/0dbdaa0ff648/Lib/">browse</a></li>
</ul>
<ul>
<li class="active">file</li>
<li><a href="/cpython/file/tip/Lib/random.py">latest</a></li>
<li><a href="/cpython/diff/0dbdaa0ff648/Lib/random.py">diff</a></li>
<li><a href="/cpython/annotate/0dbdaa0ff648/Lib/random.py">annotate</a></li>
<li><a href="/cpython/log/0dbdaa0ff648/Lib/random.py">file log</a></li>
<li><a href="/cpython/raw-file/0dbdaa0ff648/Lib/random.py">raw</a></li>
</ul>
<ul>
<li><a href="/cpython/help">help</a></li>
</ul>
</div>

<div class="main">
<h2><a href="/cpython/">cpython</a></h2>
<h3>view Lib/random.py @ 78577:0dbdaa0ff648</h3>

<form class="search" action="/cpython/log">

<p><input name="rev" id="search1" type="text" size="30" /></p>
<div id="hint">find changesets by author, revision,
files, or words in the commit message</div>
</form>

<div class="description">fix typo</div>

<table id="changesetEntry">
<tr>
 <th class="author">author</th>
 <td class="author">&#83;&#97;&#110;&#100;&#114;&#111;&#32;&#84;&#111;&#115;&#105;&#32;&#60;&#115;&#97;&#110;&#100;&#114;&#111;&#46;&#116;&#111;&#115;&#105;&#64;&#103;&#109;&#97;&#105;&#108;&#46;&#99;&#111;&#109;&#62;</td>
</tr>
<tr>
 <th class="date">date</th>
 <td class="date age">Tue, 14 Aug 2012 19:51:31 +0200</td>
</tr>
<tr>
 <th class="author">parents</th>
 <td class="author"><a href="/cpython/file/828e29cee88e/Lib/random.py">828e29cee88e</a> </td>
</tr>
<tr>
 <th class="author">children</th>
 <td class="author"></td>
</tr>

</table>

<div class="overflow">
<div class="sourcefirst"> line source</div>

<div class="parity0 source"><a href="#l1" id="l1">     1</a> <span class="sd">&quot;&quot;&quot;Random variable generators.</span></div>
<div class="parity1 source"><a href="#l2" id="l2">     2</a> </div>
<div class="parity0 source"><a href="#l3" id="l3">     3</a> <span class="sd">    integers</span></div>
<div class="parity1 source"><a href="#l4" id="l4">     4</a> <span class="sd">    --------</span></div>
<div class="parity0 source"><a href="#l5" id="l5">     5</a> <span class="sd">           uniform within range</span></div>
<div class="parity1 source"><a href="#l6" id="l6">     6</a> </div>
<div class="parity0 source"><a href="#l7" id="l7">     7</a> <span class="sd">    sequences</span></div>
<div class="parity1 source"><a href="#l8" id="l8">     8</a> <span class="sd">    ---------</span></div>
<div class="parity0 source"><a href="#l9" id="l9">     9</a> <span class="sd">           pick random element</span></div>
<div class="parity1 source"><a href="#l10" id="l10">    10</a> <span class="sd">           pick random sample</span></div>
<div class="parity0 source"><a href="#l11" id="l11">    11</a> <span class="sd">           generate random permutation</span></div>
<div class="parity1 source"><a href="#l12" id="l12">    12</a> </div>
<div class="parity0 source"><a href="#l13" id="l13">    13</a> <span class="sd">    distributions on the real line:</span></div>
<div class="parity1 source"><a href="#l14" id="l14">    14</a> <span class="sd">    ------------------------------</span></div>
<div class="parity0 source"><a href="#l15" id="l15">    15</a> <span class="sd">           uniform</span></div>
<div class="parity1 source"><a href="#l16" id="l16">    16</a> <span class="sd">           triangular</span></div>
<div class="parity0 source"><a href="#l17" id="l17">    17</a> <span class="sd">           normal (Gaussian)</span></div>
<div class="parity1 source"><a href="#l18" id="l18">    18</a> <span class="sd">           lognormal</span></div>
<div class="parity0 source"><a href="#l19" id="l19">    19</a> <span class="sd">           negative exponential</span></div>
<div class="parity1 source"><a href="#l20" id="l20">    20</a> <span class="sd">           gamma</span></div>
<div class="parity0 source"><a href="#l21" id="l21">    21</a> <span class="sd">           beta</span></div>
<div class="parity1 source"><a href="#l22" id="l22">    22</a> <span class="sd">           pareto</span></div>
<div class="parity0 source"><a href="#l23" id="l23">    23</a> <span class="sd">           Weibull</span></div>
<div class="parity1 source"><a href="#l24" id="l24">    24</a> </div>
<div class="parity0 source"><a href="#l25" id="l25">    25</a> <span class="sd">    distributions on the circle (angles 0 to 2pi)</span></div>
<div class="parity1 source"><a href="#l26" id="l26">    26</a> <span class="sd">    ---------------------------------------------</span></div>
<div class="parity0 source"><a href="#l27" id="l27">    27</a> <span class="sd">           circular uniform</span></div>
<div class="parity1 source"><a href="#l28" id="l28">    28</a> <span class="sd">           von Mises</span></div>
<div class="parity0 source"><a href="#l29" id="l29">    29</a> </div>
<div class="parity1 source"><a href="#l30" id="l30">    30</a> <span class="sd">General notes on the underlying Mersenne Twister core generator:</span></div>
<div class="parity0 source"><a href="#l31" id="l31">    31</a> </div>
<div class="parity1 source"><a href="#l32" id="l32">    32</a> <span class="sd">* The period is 2**19937-1.</span></div>
<div class="parity0 source"><a href="#l33" id="l33">    33</a> <span class="sd">* It is one of the most extensively tested generators in existence.</span></div>
<div class="parity1 source"><a href="#l34" id="l34">    34</a> <span class="sd">* Without a direct way to compute N steps forward, the semantics of</span></div>
<div class="parity0 source"><a href="#l35" id="l35">    35</a> <span class="sd">  jumpahead(n) are weakened to simply jump to another distant state and rely</span></div>
<div class="parity1 source"><a href="#l36" id="l36">    36</a> <span class="sd">  on the large period to avoid overlapping sequences.</span></div>
<div class="parity0 source"><a href="#l37" id="l37">    37</a> <span class="sd">* The random() method is implemented in C, executes in a single Python step,</span></div>
<div class="parity1 source"><a href="#l38" id="l38">    38</a> <span class="sd">  and is, therefore, threadsafe.</span></div>
<div class="parity0 source"><a href="#l39" id="l39">    39</a> </div>
<div class="parity1 source"><a href="#l40" id="l40">    40</a> <span class="sd">&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l41" id="l41">    41</a> </div>
<div class="parity1 source"><a href="#l42" id="l42">    42</a> <span class="kn">from</span> <span class="nn">__future__</span> <span class="kn">import</span> <span class="n">division</span></div>
<div class="parity0 source"><a href="#l43" id="l43">    43</a> <span class="kn">from</span> <span class="nn">warnings</span> <span class="kn">import</span> <span class="n">warn</span> <span class="k">as</span> <span class="n">_warn</span></div>
<div class="parity1 source"><a href="#l44" id="l44">    44</a> <span class="kn">from</span> <span class="nn">types</span> <span class="kn">import</span> <span class="n">MethodType</span> <span class="k">as</span> <span class="n">_MethodType</span><span class="p">,</span> <span class="n">BuiltinMethodType</span> <span class="k">as</span> <span class="n">_BuiltinMethodType</span></div>
<div class="parity0 source"><a href="#l45" id="l45">    45</a> <span class="kn">from</span> <span class="nn">math</span> <span class="kn">import</span> <span class="n">log</span> <span class="k">as</span> <span class="n">_log</span><span class="p">,</span> <span class="n">exp</span> <span class="k">as</span> <span class="n">_exp</span><span class="p">,</span> <span class="n">pi</span> <span class="k">as</span> <span class="n">_pi</span><span class="p">,</span> <span class="n">e</span> <span class="k">as</span> <span class="n">_e</span><span class="p">,</span> <span class="n">ceil</span> <span class="k">as</span> <span class="n">_ceil</span></div>
<div class="parity1 source"><a href="#l46" id="l46">    46</a> <span class="kn">from</span> <span class="nn">math</span> <span class="kn">import</span> <span class="n">sqrt</span> <span class="k">as</span> <span class="n">_sqrt</span><span class="p">,</span> <span class="n">acos</span> <span class="k">as</span> <span class="n">_acos</span><span class="p">,</span> <span class="n">cos</span> <span class="k">as</span> <span class="n">_cos</span><span class="p">,</span> <span class="n">sin</span> <span class="k">as</span> <span class="n">_sin</span></div>
<div class="parity0 source"><a href="#l47" id="l47">    47</a> <span class="kn">from</span> <span class="nn">os</span> <span class="kn">import</span> <span class="n">urandom</span> <span class="k">as</span> <span class="n">_urandom</span></div>
<div class="parity1 source"><a href="#l48" id="l48">    48</a> <span class="kn">from</span> <span class="nn">binascii</span> <span class="kn">import</span> <span class="n">hexlify</span> <span class="k">as</span> <span class="n">_hexlify</span></div>
<div class="parity0 source"><a href="#l49" id="l49">    49</a> <span class="kn">import</span> <span class="nn">hashlib</span> <span class="kn">as</span> <span class="nn">_hashlib</span></div>
<div class="parity1 source"><a href="#l50" id="l50">    50</a> </div>
<div class="parity0 source"><a href="#l51" id="l51">    51</a> <span class="n">__all__</span> <span class="o">=</span> <span class="p">[</span><span class="s">&quot;Random&quot;</span><span class="p">,</span><span class="s">&quot;seed&quot;</span><span class="p">,</span><span class="s">&quot;random&quot;</span><span class="p">,</span><span class="s">&quot;uniform&quot;</span><span class="p">,</span><span class="s">&quot;randint&quot;</span><span class="p">,</span><span class="s">&quot;choice&quot;</span><span class="p">,</span><span class="s">&quot;sample&quot;</span><span class="p">,</span></div>
<div class="parity1 source"><a href="#l52" id="l52">    52</a>            <span class="s">&quot;randrange&quot;</span><span class="p">,</span><span class="s">&quot;shuffle&quot;</span><span class="p">,</span><span class="s">&quot;normalvariate&quot;</span><span class="p">,</span><span class="s">&quot;lognormvariate&quot;</span><span class="p">,</span></div>
<div class="parity0 source"><a href="#l53" id="l53">    53</a>            <span class="s">&quot;expovariate&quot;</span><span class="p">,</span><span class="s">&quot;vonmisesvariate&quot;</span><span class="p">,</span><span class="s">&quot;gammavariate&quot;</span><span class="p">,</span><span class="s">&quot;triangular&quot;</span><span class="p">,</span></div>
<div class="parity1 source"><a href="#l54" id="l54">    54</a>            <span class="s">&quot;gauss&quot;</span><span class="p">,</span><span class="s">&quot;betavariate&quot;</span><span class="p">,</span><span class="s">&quot;paretovariate&quot;</span><span class="p">,</span><span class="s">&quot;weibullvariate&quot;</span><span class="p">,</span></div>
<div class="parity0 source"><a href="#l55" id="l55">    55</a>            <span class="s">&quot;getstate&quot;</span><span class="p">,</span><span class="s">&quot;setstate&quot;</span><span class="p">,</span><span class="s">&quot;jumpahead&quot;</span><span class="p">,</span> <span class="s">&quot;WichmannHill&quot;</span><span class="p">,</span> <span class="s">&quot;getrandbits&quot;</span><span class="p">,</span></div>
<div class="parity1 source"><a href="#l56" id="l56">    56</a>            <span class="s">&quot;SystemRandom&quot;</span><span class="p">]</span></div>
<div class="parity0 source"><a href="#l57" id="l57">    57</a> </div>
<div class="parity1 source"><a href="#l58" id="l58">    58</a> <span class="n">NV_MAGICCONST</span> <span class="o">=</span> <span class="mi">4</span> <span class="o">*</span> <span class="n">_exp</span><span class="p">(</span><span class="o">-</span><span class="mf">0.5</span><span class="p">)</span><span class="o">/</span><span class="n">_sqrt</span><span class="p">(</span><span class="mf">2.0</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l59" id="l59">    59</a> <span class="n">TWOPI</span> <span class="o">=</span> <span class="mf">2.0</span><span class="o">*</span><span class="n">_pi</span></div>
<div class="parity1 source"><a href="#l60" id="l60">    60</a> <span class="n">LOG4</span> <span class="o">=</span> <span class="n">_log</span><span class="p">(</span><span class="mf">4.0</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l61" id="l61">    61</a> <span class="n">SG_MAGICCONST</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">+</span> <span class="n">_log</span><span class="p">(</span><span class="mf">4.5</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l62" id="l62">    62</a> <span class="n">BPF</span> <span class="o">=</span> <span class="mi">53</span>        <span class="c"># Number of bits in a float</span></div>
<div class="parity0 source"><a href="#l63" id="l63">    63</a> <span class="n">RECIP_BPF</span> <span class="o">=</span> <span class="mi">2</span><span class="o">**-</span><span class="n">BPF</span></div>
<div class="parity1 source"><a href="#l64" id="l64">    64</a> </div>
<div class="parity0 source"><a href="#l65" id="l65">    65</a> </div>
<div class="parity1 source"><a href="#l66" id="l66">    66</a> <span class="c"># Translated by Guido van Rossum from C source provided by</span></div>
<div class="parity0 source"><a href="#l67" id="l67">    67</a> <span class="c"># Adrian Baddeley.  Adapted by Raymond Hettinger for use with</span></div>
<div class="parity1 source"><a href="#l68" id="l68">    68</a> <span class="c"># the Mersenne Twister  and os.urandom() core generators.</span></div>
<div class="parity0 source"><a href="#l69" id="l69">    69</a> </div>
<div class="parity1 source"><a href="#l70" id="l70">    70</a> <span class="kn">import</span> <span class="nn">_random</span></div>
<div class="parity0 source"><a href="#l71" id="l71">    71</a> </div>
<div class="parity1 source"><a href="#l72" id="l72">    72</a> <span class="k">class</span> <span class="nc">Random</span><span class="p">(</span><span class="n">_random</span><span class="o">.</span><span class="n">Random</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l73" id="l73">    73</a>     <span class="sd">&quot;&quot;&quot;Random number generator base class used by bound module functions.</span></div>
<div class="parity1 source"><a href="#l74" id="l74">    74</a> </div>
<div class="parity0 source"><a href="#l75" id="l75">    75</a> <span class="sd">    Used to instantiate instances of Random to get generators that don&#39;t</span></div>
<div class="parity1 source"><a href="#l76" id="l76">    76</a> <span class="sd">    share state.  Especially useful for multi-threaded programs, creating</span></div>
<div class="parity0 source"><a href="#l77" id="l77">    77</a> <span class="sd">    a different instance of Random for each thread, and using the jumpahead()</span></div>
<div class="parity1 source"><a href="#l78" id="l78">    78</a> <span class="sd">    method to ensure that the generated sequences seen by each thread don&#39;t</span></div>
<div class="parity0 source"><a href="#l79" id="l79">    79</a> <span class="sd">    overlap.</span></div>
<div class="parity1 source"><a href="#l80" id="l80">    80</a> </div>
<div class="parity0 source"><a href="#l81" id="l81">    81</a> <span class="sd">    Class Random can also be subclassed if you want to use a different basic</span></div>
<div class="parity1 source"><a href="#l82" id="l82">    82</a> <span class="sd">    generator of your own devising: in that case, override the following</span></div>
<div class="parity0 source"><a href="#l83" id="l83">    83</a> <span class="sd">    methods: random(), seed(), getstate(), setstate() and jumpahead().</span></div>
<div class="parity1 source"><a href="#l84" id="l84">    84</a> <span class="sd">    Optionally, implement a getrandbits() method so that randrange() can cover</span></div>
<div class="parity0 source"><a href="#l85" id="l85">    85</a> <span class="sd">    arbitrarily large ranges.</span></div>
<div class="parity1 source"><a href="#l86" id="l86">    86</a> </div>
<div class="parity0 source"><a href="#l87" id="l87">    87</a> <span class="sd">    &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l88" id="l88">    88</a> </div>
<div class="parity0 source"><a href="#l89" id="l89">    89</a>     <span class="n">VERSION</span> <span class="o">=</span> <span class="mi">3</span>     <span class="c"># used by getstate/setstate</span></div>
<div class="parity1 source"><a href="#l90" id="l90">    90</a> </div>
<div class="parity0 source"><a href="#l91" id="l91">    91</a>     <span class="k">def</span> <span class="nf">__init__</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">x</span><span class="o">=</span><span class="bp">None</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l92" id="l92">    92</a>         <span class="sd">&quot;&quot;&quot;Initialize an instance.</span></div>
<div class="parity0 source"><a href="#l93" id="l93">    93</a> </div>
<div class="parity1 source"><a href="#l94" id="l94">    94</a> <span class="sd">        Optional argument x controls seeding, as for Random.seed().</span></div>
<div class="parity0 source"><a href="#l95" id="l95">    95</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l96" id="l96">    96</a> </div>
<div class="parity0 source"><a href="#l97" id="l97">    97</a>         <span class="bp">self</span><span class="o">.</span><span class="n">seed</span><span class="p">(</span><span class="n">x</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l98" id="l98">    98</a>         <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="bp">None</span></div>
<div class="parity0 source"><a href="#l99" id="l99">    99</a> </div>
<div class="parity1 source"><a href="#l100" id="l100">   100</a>     <span class="k">def</span> <span class="nf">seed</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">a</span><span class="o">=</span><span class="bp">None</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l101" id="l101">   101</a>         <span class="sd">&quot;&quot;&quot;Initialize internal state from hashable object.</span></div>
<div class="parity1 source"><a href="#l102" id="l102">   102</a> </div>
<div class="parity0 source"><a href="#l103" id="l103">   103</a> <span class="sd">        None or no argument seeds from current time or from an operating</span></div>
<div class="parity1 source"><a href="#l104" id="l104">   104</a> <span class="sd">        system specific randomness source if available.</span></div>
<div class="parity0 source"><a href="#l105" id="l105">   105</a> </div>
<div class="parity1 source"><a href="#l106" id="l106">   106</a> <span class="sd">        If a is not None or an int or long, hash(a) is used instead.</span></div>
<div class="parity0 source"><a href="#l107" id="l107">   107</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l108" id="l108">   108</a> </div>
<div class="parity0 source"><a href="#l109" id="l109">   109</a>         <span class="k">if</span> <span class="n">a</span> <span class="ow">is</span> <span class="bp">None</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l110" id="l110">   110</a>             <span class="k">try</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l111" id="l111">   111</a>                 <span class="n">a</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">_hexlify</span><span class="p">(</span><span class="n">_urandom</span><span class="p">(</span><span class="mi">16</span><span class="p">)),</span> <span class="mi">16</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l112" id="l112">   112</a>             <span class="k">except</span> <span class="ne">NotImplementedError</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l113" id="l113">   113</a>                 <span class="kn">import</span> <span class="nn">time</span></div>
<div class="parity1 source"><a href="#l114" id="l114">   114</a>                 <span class="n">a</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">time</span><span class="o">.</span><span class="n">time</span><span class="p">()</span> <span class="o">*</span> <span class="mi">256</span><span class="p">)</span> <span class="c"># use fractional seconds</span></div>
<div class="parity0 source"><a href="#l115" id="l115">   115</a> </div>
<div class="parity1 source"><a href="#l116" id="l116">   116</a>         <span class="nb">super</span><span class="p">(</span><span class="n">Random</span><span class="p">,</span> <span class="bp">self</span><span class="p">)</span><span class="o">.</span><span class="n">seed</span><span class="p">(</span><span class="n">a</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l117" id="l117">   117</a>         <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="bp">None</span></div>
<div class="parity1 source"><a href="#l118" id="l118">   118</a> </div>
<div class="parity0 source"><a href="#l119" id="l119">   119</a>     <span class="k">def</span> <span class="nf">getstate</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l120" id="l120">   120</a>         <span class="sd">&quot;&quot;&quot;Return internal state; can be passed to setstate() later.&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l121" id="l121">   121</a>         <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">VERSION</span><span class="p">,</span> <span class="nb">super</span><span class="p">(</span><span class="n">Random</span><span class="p">,</span> <span class="bp">self</span><span class="p">)</span><span class="o">.</span><span class="n">getstate</span><span class="p">(),</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span></div>
<div class="parity1 source"><a href="#l122" id="l122">   122</a> </div>
<div class="parity0 source"><a href="#l123" id="l123">   123</a>     <span class="k">def</span> <span class="nf">setstate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">state</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l124" id="l124">   124</a>         <span class="sd">&quot;&quot;&quot;Restore internal state from object returned by getstate().&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l125" id="l125">   125</a>         <span class="n">version</span> <span class="o">=</span> <span class="n">state</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span></div>
<div class="parity1 source"><a href="#l126" id="l126">   126</a>         <span class="k">if</span> <span class="n">version</span> <span class="o">==</span> <span class="mi">3</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l127" id="l127">   127</a>             <span class="n">version</span><span class="p">,</span> <span class="n">internalstate</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="n">state</span></div>
<div class="parity1 source"><a href="#l128" id="l128">   128</a>             <span class="nb">super</span><span class="p">(</span><span class="n">Random</span><span class="p">,</span> <span class="bp">self</span><span class="p">)</span><span class="o">.</span><span class="n">setstate</span><span class="p">(</span><span class="n">internalstate</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l129" id="l129">   129</a>         <span class="k">elif</span> <span class="n">version</span> <span class="o">==</span> <span class="mi">2</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l130" id="l130">   130</a>             <span class="n">version</span><span class="p">,</span> <span class="n">internalstate</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="n">state</span></div>
<div class="parity0 source"><a href="#l131" id="l131">   131</a>             <span class="c"># In version 2, the state was saved as signed ints, which causes</span></div>
<div class="parity1 source"><a href="#l132" id="l132">   132</a>             <span class="c">#   inconsistencies between 32/64-bit systems. The state is</span></div>
<div class="parity0 source"><a href="#l133" id="l133">   133</a>             <span class="c">#   really unsigned 32-bit ints, so we convert negative ints from</span></div>
<div class="parity1 source"><a href="#l134" id="l134">   134</a>             <span class="c">#   version 2 to positive longs for version 3.</span></div>
<div class="parity0 source"><a href="#l135" id="l135">   135</a>             <span class="k">try</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l136" id="l136">   136</a>                 <span class="n">internalstate</span> <span class="o">=</span> <span class="nb">tuple</span><span class="p">(</span> <span class="nb">long</span><span class="p">(</span><span class="n">x</span><span class="p">)</span> <span class="o">%</span> <span class="p">(</span><span class="mi">2</span><span class="o">**</span><span class="mi">32</span><span class="p">)</span> <span class="k">for</span> <span class="n">x</span> <span class="ow">in</span> <span class="n">internalstate</span> <span class="p">)</span></div>
<div class="parity0 source"><a href="#l137" id="l137">   137</a>             <span class="k">except</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="n">e</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l138" id="l138">   138</a>                 <span class="k">raise</span> <span class="ne">TypeError</span><span class="p">,</span> <span class="n">e</span></div>
<div class="parity0 source"><a href="#l139" id="l139">   139</a>             <span class="nb">super</span><span class="p">(</span><span class="n">Random</span><span class="p">,</span> <span class="bp">self</span><span class="p">)</span><span class="o">.</span><span class="n">setstate</span><span class="p">(</span><span class="n">internalstate</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l140" id="l140">   140</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l141" id="l141">   141</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&quot;state with version </span><span class="si">%s</span><span class="s"> passed to &quot;</span></div>
<div class="parity1 source"><a href="#l142" id="l142">   142</a>                              <span class="s">&quot;Random.setstate() of version </span><span class="si">%s</span><span class="s">&quot;</span> <span class="o">%</span></div>
<div class="parity0 source"><a href="#l143" id="l143">   143</a>                              <span class="p">(</span><span class="n">version</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">VERSION</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l144" id="l144">   144</a> </div>
<div class="parity0 source"><a href="#l145" id="l145">   145</a>     <span class="k">def</span> <span class="nf">jumpahead</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">n</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l146" id="l146">   146</a>         <span class="sd">&quot;&quot;&quot;Change the internal state to one that is likely far away</span></div>
<div class="parity0 source"><a href="#l147" id="l147">   147</a> <span class="sd">        from the current state.  This method will not be in Py3.x,</span></div>
<div class="parity1 source"><a href="#l148" id="l148">   148</a> <span class="sd">        so it is better to simply reseed.</span></div>
<div class="parity0 source"><a href="#l149" id="l149">   149</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l150" id="l150">   150</a>         <span class="c"># The super.jumpahead() method uses shuffling to change state,</span></div>
<div class="parity0 source"><a href="#l151" id="l151">   151</a>         <span class="c"># so it needs a large and &quot;interesting&quot; n to work with.  Here,</span></div>
<div class="parity1 source"><a href="#l152" id="l152">   152</a>         <span class="c"># we use hashing to create a large n for the shuffle.</span></div>
<div class="parity0 source"><a href="#l153" id="l153">   153</a>         <span class="n">s</span> <span class="o">=</span> <span class="nb">repr</span><span class="p">(</span><span class="n">n</span><span class="p">)</span> <span class="o">+</span> <span class="nb">repr</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">getstate</span><span class="p">())</span></div>
<div class="parity1 source"><a href="#l154" id="l154">   154</a>         <span class="n">n</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">_hashlib</span><span class="o">.</span><span class="n">new</span><span class="p">(</span><span class="s">&#39;sha512&#39;</span><span class="p">,</span> <span class="n">s</span><span class="p">)</span><span class="o">.</span><span class="n">hexdigest</span><span class="p">(),</span> <span class="mi">16</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l155" id="l155">   155</a>         <span class="nb">super</span><span class="p">(</span><span class="n">Random</span><span class="p">,</span> <span class="bp">self</span><span class="p">)</span><span class="o">.</span><span class="n">jumpahead</span><span class="p">(</span><span class="n">n</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l156" id="l156">   156</a> </div>
<div class="parity0 source"><a href="#l157" id="l157">   157</a> <span class="c">## ---- Methods below this point do not need to be overridden when</span></div>
<div class="parity1 source"><a href="#l158" id="l158">   158</a> <span class="c">## ---- subclassing for the purpose of using a different core generator.</span></div>
<div class="parity0 source"><a href="#l159" id="l159">   159</a> </div>
<div class="parity1 source"><a href="#l160" id="l160">   160</a> <span class="c">## -------------------- pickle support  -------------------</span></div>
<div class="parity0 source"><a href="#l161" id="l161">   161</a> </div>
<div class="parity1 source"><a href="#l162" id="l162">   162</a>     <span class="k">def</span> <span class="nf">__getstate__</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span> <span class="c"># for pickle</span></div>
<div class="parity0 source"><a href="#l163" id="l163">   163</a>         <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">getstate</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l164" id="l164">   164</a> </div>
<div class="parity0 source"><a href="#l165" id="l165">   165</a>     <span class="k">def</span> <span class="nf">__setstate__</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">state</span><span class="p">):</span>  <span class="c"># for pickle</span></div>
<div class="parity1 source"><a href="#l166" id="l166">   166</a>         <span class="bp">self</span><span class="o">.</span><span class="n">setstate</span><span class="p">(</span><span class="n">state</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l167" id="l167">   167</a> </div>
<div class="parity1 source"><a href="#l168" id="l168">   168</a>     <span class="k">def</span> <span class="nf">__reduce__</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l169" id="l169">   169</a>         <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">__class__</span><span class="p">,</span> <span class="p">(),</span> <span class="bp">self</span><span class="o">.</span><span class="n">getstate</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l170" id="l170">   170</a> </div>
<div class="parity0 source"><a href="#l171" id="l171">   171</a> <span class="c">## -------------------- integer methods  -------------------</span></div>
<div class="parity1 source"><a href="#l172" id="l172">   172</a> </div>
<div class="parity0 source"><a href="#l173" id="l173">   173</a>     <span class="k">def</span> <span class="nf">randrange</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">start</span><span class="p">,</span> <span class="n">stop</span><span class="o">=</span><span class="bp">None</span><span class="p">,</span> <span class="n">step</span><span class="o">=</span><span class="mi">1</span><span class="p">,</span> <span class="nb">int</span><span class="o">=</span><span class="nb">int</span><span class="p">,</span> <span class="n">default</span><span class="o">=</span><span class="bp">None</span><span class="p">,</span></div>
<div class="parity1 source"><a href="#l174" id="l174">   174</a>                   <span class="n">maxwidth</span><span class="o">=</span><span class="il">1L</span><span class="o">&lt;&lt;</span><span class="n">BPF</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l175" id="l175">   175</a>         <span class="sd">&quot;&quot;&quot;Choose a random item from range(start, stop[, step]).</span></div>
<div class="parity1 source"><a href="#l176" id="l176">   176</a> </div>
<div class="parity0 source"><a href="#l177" id="l177">   177</a> <span class="sd">        This fixes the problem with randint() which includes the</span></div>
<div class="parity1 source"><a href="#l178" id="l178">   178</a> <span class="sd">        endpoint; in Python this is usually not what you want.</span></div>
<div class="parity0 source"><a href="#l179" id="l179">   179</a> <span class="sd">        Do not supply the &#39;int&#39;, &#39;default&#39;, and &#39;maxwidth&#39; arguments.</span></div>
<div class="parity1 source"><a href="#l180" id="l180">   180</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l181" id="l181">   181</a> </div>
<div class="parity1 source"><a href="#l182" id="l182">   182</a>         <span class="c"># This code is a bit messy to make it fast for the</span></div>
<div class="parity0 source"><a href="#l183" id="l183">   183</a>         <span class="c"># common case while still doing adequate error checking.</span></div>
<div class="parity1 source"><a href="#l184" id="l184">   184</a>         <span class="n">istart</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">start</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l185" id="l185">   185</a>         <span class="k">if</span> <span class="n">istart</span> <span class="o">!=</span> <span class="n">start</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l186" id="l186">   186</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;non-integer arg 1 for randrange()&quot;</span></div>
<div class="parity0 source"><a href="#l187" id="l187">   187</a>         <span class="k">if</span> <span class="n">stop</span> <span class="ow">is</span> <span class="n">default</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l188" id="l188">   188</a>             <span class="k">if</span> <span class="n">istart</span> <span class="o">&gt;</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l189" id="l189">   189</a>                 <span class="k">if</span> <span class="n">istart</span> <span class="o">&gt;=</span> <span class="n">maxwidth</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l190" id="l190">   190</a>                     <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">_randbelow</span><span class="p">(</span><span class="n">istart</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l191" id="l191">   191</a>                 <span class="k">return</span> <span class="nb">int</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">istart</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l192" id="l192">   192</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;empty range for randrange()&quot;</span></div>
<div class="parity0 source"><a href="#l193" id="l193">   193</a> </div>
<div class="parity1 source"><a href="#l194" id="l194">   194</a>         <span class="c"># stop argument supplied.</span></div>
<div class="parity0 source"><a href="#l195" id="l195">   195</a>         <span class="n">istop</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">stop</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l196" id="l196">   196</a>         <span class="k">if</span> <span class="n">istop</span> <span class="o">!=</span> <span class="n">stop</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l197" id="l197">   197</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;non-integer stop for randrange()&quot;</span></div>
<div class="parity1 source"><a href="#l198" id="l198">   198</a>         <span class="n">width</span> <span class="o">=</span> <span class="n">istop</span> <span class="o">-</span> <span class="n">istart</span></div>
<div class="parity0 source"><a href="#l199" id="l199">   199</a>         <span class="k">if</span> <span class="n">step</span> <span class="o">==</span> <span class="mi">1</span> <span class="ow">and</span> <span class="n">width</span> <span class="o">&gt;</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l200" id="l200">   200</a>             <span class="c"># Note that</span></div>
<div class="parity0 source"><a href="#l201" id="l201">   201</a>             <span class="c">#     int(istart + self.random()*width)</span></div>
<div class="parity1 source"><a href="#l202" id="l202">   202</a>             <span class="c"># instead would be incorrect.  For example, consider istart</span></div>
<div class="parity0 source"><a href="#l203" id="l203">   203</a>             <span class="c"># = -2 and istop = 0.  Then the guts would be in</span></div>
<div class="parity1 source"><a href="#l204" id="l204">   204</a>             <span class="c"># -2.0 to 0.0 exclusive on both ends (ignoring that random()</span></div>
<div class="parity0 source"><a href="#l205" id="l205">   205</a>             <span class="c"># might return 0.0), and because int() truncates toward 0, the</span></div>
<div class="parity1 source"><a href="#l206" id="l206">   206</a>             <span class="c"># final result would be -1 or 0 (instead of -2 or -1).</span></div>
<div class="parity0 source"><a href="#l207" id="l207">   207</a>             <span class="c">#     istart + int(self.random()*width)</span></div>
<div class="parity1 source"><a href="#l208" id="l208">   208</a>             <span class="c"># would also be incorrect, for a subtler reason:  the RHS</span></div>
<div class="parity0 source"><a href="#l209" id="l209">   209</a>             <span class="c"># can return a long, and then randrange() would also return</span></div>
<div class="parity1 source"><a href="#l210" id="l210">   210</a>             <span class="c"># a long, but we&#39;re supposed to return an int (for backward</span></div>
<div class="parity0 source"><a href="#l211" id="l211">   211</a>             <span class="c"># compatibility).</span></div>
<div class="parity1 source"><a href="#l212" id="l212">   212</a> </div>
<div class="parity0 source"><a href="#l213" id="l213">   213</a>             <span class="k">if</span> <span class="n">width</span> <span class="o">&gt;=</span> <span class="n">maxwidth</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l214" id="l214">   214</a>                 <span class="k">return</span> <span class="nb">int</span><span class="p">(</span><span class="n">istart</span> <span class="o">+</span> <span class="bp">self</span><span class="o">.</span><span class="n">_randbelow</span><span class="p">(</span><span class="n">width</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l215" id="l215">   215</a>             <span class="k">return</span> <span class="nb">int</span><span class="p">(</span><span class="n">istart</span> <span class="o">+</span> <span class="nb">int</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span><span class="o">*</span><span class="n">width</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l216" id="l216">   216</a>         <span class="k">if</span> <span class="n">step</span> <span class="o">==</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l217" id="l217">   217</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;empty range for randrange() (</span><span class="si">%d</span><span class="s">,</span><span class="si">%d</span><span class="s">, </span><span class="si">%d</span><span class="s">)&quot;</span> <span class="o">%</span> <span class="p">(</span><span class="n">istart</span><span class="p">,</span> <span class="n">istop</span><span class="p">,</span> <span class="n">width</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l218" id="l218">   218</a> </div>
<div class="parity0 source"><a href="#l219" id="l219">   219</a>         <span class="c"># Non-unit step argument supplied.</span></div>
<div class="parity1 source"><a href="#l220" id="l220">   220</a>         <span class="n">istep</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">step</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l221" id="l221">   221</a>         <span class="k">if</span> <span class="n">istep</span> <span class="o">!=</span> <span class="n">step</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l222" id="l222">   222</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;non-integer step for randrange()&quot;</span></div>
<div class="parity0 source"><a href="#l223" id="l223">   223</a>         <span class="k">if</span> <span class="n">istep</span> <span class="o">&gt;</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l224" id="l224">   224</a>             <span class="n">n</span> <span class="o">=</span> <span class="p">(</span><span class="n">width</span> <span class="o">+</span> <span class="n">istep</span> <span class="o">-</span> <span class="mi">1</span><span class="p">)</span> <span class="o">//</span> <span class="n">istep</span></div>
<div class="parity0 source"><a href="#l225" id="l225">   225</a>         <span class="k">elif</span> <span class="n">istep</span> <span class="o">&lt;</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l226" id="l226">   226</a>             <span class="n">n</span> <span class="o">=</span> <span class="p">(</span><span class="n">width</span> <span class="o">+</span> <span class="n">istep</span> <span class="o">+</span> <span class="mi">1</span><span class="p">)</span> <span class="o">//</span> <span class="n">istep</span></div>
<div class="parity0 source"><a href="#l227" id="l227">   227</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l228" id="l228">   228</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;zero step for randrange()&quot;</span></div>
<div class="parity0 source"><a href="#l229" id="l229">   229</a> </div>
<div class="parity1 source"><a href="#l230" id="l230">   230</a>         <span class="k">if</span> <span class="n">n</span> <span class="o">&lt;=</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l231" id="l231">   231</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&quot;empty range for randrange()&quot;</span></div>
<div class="parity1 source"><a href="#l232" id="l232">   232</a> </div>
<div class="parity0 source"><a href="#l233" id="l233">   233</a>         <span class="k">if</span> <span class="n">n</span> <span class="o">&gt;=</span> <span class="n">maxwidth</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l234" id="l234">   234</a>             <span class="k">return</span> <span class="n">istart</span> <span class="o">+</span> <span class="n">istep</span><span class="o">*</span><span class="bp">self</span><span class="o">.</span><span class="n">_randbelow</span><span class="p">(</span><span class="n">n</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l235" id="l235">   235</a>         <span class="k">return</span> <span class="n">istart</span> <span class="o">+</span> <span class="n">istep</span><span class="o">*</span><span class="nb">int</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">n</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l236" id="l236">   236</a> </div>
<div class="parity0 source"><a href="#l237" id="l237">   237</a>     <span class="k">def</span> <span class="nf">randint</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l238" id="l238">   238</a>         <span class="sd">&quot;&quot;&quot;Return random integer in range [a, b], including both end points.</span></div>
<div class="parity0 source"><a href="#l239" id="l239">   239</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l240" id="l240">   240</a> </div>
<div class="parity0 source"><a href="#l241" id="l241">   241</a>         <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">randrange</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="o">+</span><span class="mi">1</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l242" id="l242">   242</a> </div>
<div class="parity0 source"><a href="#l243" id="l243">   243</a>     <span class="k">def</span> <span class="nf">_randbelow</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">n</span><span class="p">,</span> <span class="n">_log</span><span class="o">=</span><span class="n">_log</span><span class="p">,</span> <span class="nb">int</span><span class="o">=</span><span class="nb">int</span><span class="p">,</span> <span class="n">_maxwidth</span><span class="o">=</span><span class="il">1L</span><span class="o">&lt;&lt;</span><span class="n">BPF</span><span class="p">,</span></div>
<div class="parity1 source"><a href="#l244" id="l244">   244</a>                    <span class="n">_Method</span><span class="o">=</span><span class="n">_MethodType</span><span class="p">,</span> <span class="n">_BuiltinMethod</span><span class="o">=</span><span class="n">_BuiltinMethodType</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l245" id="l245">   245</a>         <span class="sd">&quot;&quot;&quot;Return a random int in the range [0,n)</span></div>
<div class="parity1 source"><a href="#l246" id="l246">   246</a> </div>
<div class="parity0 source"><a href="#l247" id="l247">   247</a> <span class="sd">        Handles the case where n has more bits than returned</span></div>
<div class="parity1 source"><a href="#l248" id="l248">   248</a> <span class="sd">        by a single call to the underlying generator.</span></div>
<div class="parity0 source"><a href="#l249" id="l249">   249</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l250" id="l250">   250</a> </div>
<div class="parity0 source"><a href="#l251" id="l251">   251</a>         <span class="k">try</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l252" id="l252">   252</a>             <span class="n">getrandbits</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">getrandbits</span></div>
<div class="parity0 source"><a href="#l253" id="l253">   253</a>         <span class="k">except</span> <span class="ne">AttributeError</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l254" id="l254">   254</a>             <span class="k">pass</span></div>
<div class="parity0 source"><a href="#l255" id="l255">   255</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l256" id="l256">   256</a>             <span class="c"># Only call self.getrandbits if the original random() builtin method</span></div>
<div class="parity0 source"><a href="#l257" id="l257">   257</a>             <span class="c"># has not been overridden or if a new getrandbits() was supplied.</span></div>
<div class="parity1 source"><a href="#l258" id="l258">   258</a>             <span class="c"># This assures that the two methods correspond.</span></div>
<div class="parity0 source"><a href="#l259" id="l259">   259</a>             <span class="k">if</span> <span class="nb">type</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">)</span> <span class="ow">is</span> <span class="n">_BuiltinMethod</span> <span class="ow">or</span> <span class="nb">type</span><span class="p">(</span><span class="n">getrandbits</span><span class="p">)</span> <span class="ow">is</span> <span class="n">_Method</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l260" id="l260">   260</a>                 <span class="n">k</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="mf">1.00001</span> <span class="o">+</span> <span class="n">_log</span><span class="p">(</span><span class="n">n</span><span class="o">-</span><span class="mi">1</span><span class="p">,</span> <span class="mf">2.0</span><span class="p">))</span>   <span class="c"># 2**k &gt; n-1 &gt; 2**(k-2)</span></div>
<div class="parity0 source"><a href="#l261" id="l261">   261</a>                 <span class="n">r</span> <span class="o">=</span> <span class="n">getrandbits</span><span class="p">(</span><span class="n">k</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l262" id="l262">   262</a>                 <span class="k">while</span> <span class="n">r</span> <span class="o">&gt;=</span> <span class="n">n</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l263" id="l263">   263</a>                     <span class="n">r</span> <span class="o">=</span> <span class="n">getrandbits</span><span class="p">(</span><span class="n">k</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l264" id="l264">   264</a>                 <span class="k">return</span> <span class="n">r</span></div>
<div class="parity0 source"><a href="#l265" id="l265">   265</a>         <span class="k">if</span> <span class="n">n</span> <span class="o">&gt;=</span> <span class="n">_maxwidth</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l266" id="l266">   266</a>             <span class="n">_warn</span><span class="p">(</span><span class="s">&quot;Underlying random() generator does not supply </span><span class="se">\n</span><span class="s">&quot;</span></div>
<div class="parity0 source"><a href="#l267" id="l267">   267</a>                 <span class="s">&quot;enough bits to choose from a population range this large&quot;</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l268" id="l268">   268</a>         <span class="k">return</span> <span class="nb">int</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">n</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l269" id="l269">   269</a> </div>
<div class="parity1 source"><a href="#l270" id="l270">   270</a> <span class="c">## -------------------- sequence methods  -------------------</span></div>
<div class="parity0 source"><a href="#l271" id="l271">   271</a> </div>
<div class="parity1 source"><a href="#l272" id="l272">   272</a>     <span class="k">def</span> <span class="nf">choice</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">seq</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l273" id="l273">   273</a>         <span class="sd">&quot;&quot;&quot;Choose a random element from a non-empty sequence.&quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l274" id="l274">   274</a>         <span class="k">return</span> <span class="n">seq</span><span class="p">[</span><span class="nb">int</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="nb">len</span><span class="p">(</span><span class="n">seq</span><span class="p">))]</span>  <span class="c"># raises IndexError if seq is empty</span></div>
<div class="parity0 source"><a href="#l275" id="l275">   275</a> </div>
<div class="parity1 source"><a href="#l276" id="l276">   276</a>     <span class="k">def</span> <span class="nf">shuffle</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">x</span><span class="p">,</span> <span class="n">random</span><span class="o">=</span><span class="bp">None</span><span class="p">,</span> <span class="nb">int</span><span class="o">=</span><span class="nb">int</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l277" id="l277">   277</a>         <span class="sd">&quot;&quot;&quot;x, random=random.random -&gt; shuffle list x in place; return None.</span></div>
<div class="parity1 source"><a href="#l278" id="l278">   278</a> </div>
<div class="parity0 source"><a href="#l279" id="l279">   279</a> <span class="sd">        Optional arg random is a 0-argument function returning a random</span></div>
<div class="parity1 source"><a href="#l280" id="l280">   280</a> <span class="sd">        float in [0.0, 1.0); by default, the standard random.random.</span></div>
<div class="parity0 source"><a href="#l281" id="l281">   281</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l282" id="l282">   282</a> </div>
<div class="parity0 source"><a href="#l283" id="l283">   283</a>         <span class="k">if</span> <span class="n">random</span> <span class="ow">is</span> <span class="bp">None</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l284" id="l284">   284</a>             <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity0 source"><a href="#l285" id="l285">   285</a>         <span class="k">for</span> <span class="n">i</span> <span class="ow">in</span> <span class="nb">reversed</span><span class="p">(</span><span class="nb">xrange</span><span class="p">(</span><span class="mi">1</span><span class="p">,</span> <span class="nb">len</span><span class="p">(</span><span class="n">x</span><span class="p">))):</span></div>
<div class="parity1 source"><a href="#l286" id="l286">   286</a>             <span class="c"># pick an element in x[:i+1] with which to exchange x[i]</span></div>
<div class="parity0 source"><a href="#l287" id="l287">   287</a>             <span class="n">j</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="p">(</span><span class="n">i</span><span class="o">+</span><span class="mi">1</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l288" id="l288">   288</a>             <span class="n">x</span><span class="p">[</span><span class="n">i</span><span class="p">],</span> <span class="n">x</span><span class="p">[</span><span class="n">j</span><span class="p">]</span> <span class="o">=</span> <span class="n">x</span><span class="p">[</span><span class="n">j</span><span class="p">],</span> <span class="n">x</span><span class="p">[</span><span class="n">i</span><span class="p">]</span></div>
<div class="parity0 source"><a href="#l289" id="l289">   289</a> </div>
<div class="parity1 source"><a href="#l290" id="l290">   290</a>     <span class="k">def</span> <span class="nf">sample</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">population</span><span class="p">,</span> <span class="n">k</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l291" id="l291">   291</a>         <span class="sd">&quot;&quot;&quot;Chooses k unique random elements from a population sequence.</span></div>
<div class="parity1 source"><a href="#l292" id="l292">   292</a> </div>
<div class="parity0 source"><a href="#l293" id="l293">   293</a> <span class="sd">        Returns a new list containing elements from the population while</span></div>
<div class="parity1 source"><a href="#l294" id="l294">   294</a> <span class="sd">        leaving the original population unchanged.  The resulting list is</span></div>
<div class="parity0 source"><a href="#l295" id="l295">   295</a> <span class="sd">        in selection order so that all sub-slices will also be valid random</span></div>
<div class="parity1 source"><a href="#l296" id="l296">   296</a> <span class="sd">        samples.  This allows raffle winners (the sample) to be partitioned</span></div>
<div class="parity0 source"><a href="#l297" id="l297">   297</a> <span class="sd">        into grand prize and second place winners (the subslices).</span></div>
<div class="parity1 source"><a href="#l298" id="l298">   298</a> </div>
<div class="parity0 source"><a href="#l299" id="l299">   299</a> <span class="sd">        Members of the population need not be hashable or unique.  If the</span></div>
<div class="parity1 source"><a href="#l300" id="l300">   300</a> <span class="sd">        population contains repeats, then each occurrence is a possible</span></div>
<div class="parity0 source"><a href="#l301" id="l301">   301</a> <span class="sd">        selection in the sample.</span></div>
<div class="parity1 source"><a href="#l302" id="l302">   302</a> </div>
<div class="parity0 source"><a href="#l303" id="l303">   303</a> <span class="sd">        To choose a sample in a range of integers, use xrange as an argument.</span></div>
<div class="parity1 source"><a href="#l304" id="l304">   304</a> <span class="sd">        This is especially fast and space efficient for sampling from a</span></div>
<div class="parity0 source"><a href="#l305" id="l305">   305</a> <span class="sd">        large population:   sample(xrange(10000000), 60)</span></div>
<div class="parity1 source"><a href="#l306" id="l306">   306</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l307" id="l307">   307</a> </div>
<div class="parity1 source"><a href="#l308" id="l308">   308</a>         <span class="c"># Sampling without replacement entails tracking either potential</span></div>
<div class="parity0 source"><a href="#l309" id="l309">   309</a>         <span class="c"># selections (the pool) in a list or previous selections in a set.</span></div>
<div class="parity1 source"><a href="#l310" id="l310">   310</a> </div>
<div class="parity0 source"><a href="#l311" id="l311">   311</a>         <span class="c"># When the number of selections is small compared to the</span></div>
<div class="parity1 source"><a href="#l312" id="l312">   312</a>         <span class="c"># population, then tracking selections is efficient, requiring</span></div>
<div class="parity0 source"><a href="#l313" id="l313">   313</a>         <span class="c"># only a small set and an occasional reselection.  For</span></div>
<div class="parity1 source"><a href="#l314" id="l314">   314</a>         <span class="c"># a larger number of selections, the pool tracking method is</span></div>
<div class="parity0 source"><a href="#l315" id="l315">   315</a>         <span class="c"># preferred since the list takes less space than the</span></div>
<div class="parity1 source"><a href="#l316" id="l316">   316</a>         <span class="c"># set and it doesn&#39;t suffer from frequent reselections.</span></div>
<div class="parity0 source"><a href="#l317" id="l317">   317</a> </div>
<div class="parity1 source"><a href="#l318" id="l318">   318</a>         <span class="n">n</span> <span class="o">=</span> <span class="nb">len</span><span class="p">(</span><span class="n">population</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l319" id="l319">   319</a>         <span class="k">if</span> <span class="ow">not</span> <span class="mi">0</span> <span class="o">&lt;=</span> <span class="n">k</span> <span class="o">&lt;=</span> <span class="n">n</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l320" id="l320">   320</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&quot;sample larger than population&quot;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l321" id="l321">   321</a>         <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity1 source"><a href="#l322" id="l322">   322</a>         <span class="n">_int</span> <span class="o">=</span> <span class="nb">int</span></div>
<div class="parity0 source"><a href="#l323" id="l323">   323</a>         <span class="n">result</span> <span class="o">=</span> <span class="p">[</span><span class="bp">None</span><span class="p">]</span> <span class="o">*</span> <span class="n">k</span></div>
<div class="parity1 source"><a href="#l324" id="l324">   324</a>         <span class="n">setsize</span> <span class="o">=</span> <span class="mi">21</span>        <span class="c"># size of a small set minus size of an empty list</span></div>
<div class="parity0 source"><a href="#l325" id="l325">   325</a>         <span class="k">if</span> <span class="n">k</span> <span class="o">&gt;</span> <span class="mi">5</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l326" id="l326">   326</a>             <span class="n">setsize</span> <span class="o">+=</span> <span class="mi">4</span> <span class="o">**</span> <span class="n">_ceil</span><span class="p">(</span><span class="n">_log</span><span class="p">(</span><span class="n">k</span> <span class="o">*</span> <span class="mi">3</span><span class="p">,</span> <span class="mi">4</span><span class="p">))</span> <span class="c"># table size for big sets</span></div>
<div class="parity0 source"><a href="#l327" id="l327">   327</a>         <span class="k">if</span> <span class="n">n</span> <span class="o">&lt;=</span> <span class="n">setsize</span> <span class="ow">or</span> <span class="nb">hasattr</span><span class="p">(</span><span class="n">population</span><span class="p">,</span> <span class="s">&quot;keys&quot;</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l328" id="l328">   328</a>             <span class="c"># An n-length list is smaller than a k-length set, or this is a</span></div>
<div class="parity0 source"><a href="#l329" id="l329">   329</a>             <span class="c"># mapping type so the other algorithm wouldn&#39;t work.</span></div>
<div class="parity1 source"><a href="#l330" id="l330">   330</a>             <span class="n">pool</span> <span class="o">=</span> <span class="nb">list</span><span class="p">(</span><span class="n">population</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l331" id="l331">   331</a>             <span class="k">for</span> <span class="n">i</span> <span class="ow">in</span> <span class="nb">xrange</span><span class="p">(</span><span class="n">k</span><span class="p">):</span>         <span class="c"># invariant:  non-selected at [0,n-i)</span></div>
<div class="parity1 source"><a href="#l332" id="l332">   332</a>                 <span class="n">j</span> <span class="o">=</span> <span class="n">_int</span><span class="p">(</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="p">(</span><span class="n">n</span><span class="o">-</span><span class="n">i</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l333" id="l333">   333</a>                 <span class="n">result</span><span class="p">[</span><span class="n">i</span><span class="p">]</span> <span class="o">=</span> <span class="n">pool</span><span class="p">[</span><span class="n">j</span><span class="p">]</span></div>
<div class="parity1 source"><a href="#l334" id="l334">   334</a>                 <span class="n">pool</span><span class="p">[</span><span class="n">j</span><span class="p">]</span> <span class="o">=</span> <span class="n">pool</span><span class="p">[</span><span class="n">n</span><span class="o">-</span><span class="n">i</span><span class="o">-</span><span class="mi">1</span><span class="p">]</span>   <span class="c"># move non-selected item into vacancy</span></div>
<div class="parity0 source"><a href="#l335" id="l335">   335</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l336" id="l336">   336</a>             <span class="k">try</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l337" id="l337">   337</a>                 <span class="n">selected</span> <span class="o">=</span> <span class="nb">set</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l338" id="l338">   338</a>                 <span class="n">selected_add</span> <span class="o">=</span> <span class="n">selected</span><span class="o">.</span><span class="n">add</span></div>
<div class="parity0 source"><a href="#l339" id="l339">   339</a>                 <span class="k">for</span> <span class="n">i</span> <span class="ow">in</span> <span class="nb">xrange</span><span class="p">(</span><span class="n">k</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l340" id="l340">   340</a>                     <span class="n">j</span> <span class="o">=</span> <span class="n">_int</span><span class="p">(</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">n</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l341" id="l341">   341</a>                     <span class="k">while</span> <span class="n">j</span> <span class="ow">in</span> <span class="n">selected</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l342" id="l342">   342</a>                         <span class="n">j</span> <span class="o">=</span> <span class="n">_int</span><span class="p">(</span><span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">n</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l343" id="l343">   343</a>                     <span class="n">selected_add</span><span class="p">(</span><span class="n">j</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l344" id="l344">   344</a>                     <span class="n">result</span><span class="p">[</span><span class="n">i</span><span class="p">]</span> <span class="o">=</span> <span class="n">population</span><span class="p">[</span><span class="n">j</span><span class="p">]</span></div>
<div class="parity0 source"><a href="#l345" id="l345">   345</a>             <span class="k">except</span> <span class="p">(</span><span class="ne">TypeError</span><span class="p">,</span> <span class="ne">KeyError</span><span class="p">):</span>   <span class="c"># handle (at least) sets</span></div>
<div class="parity1 source"><a href="#l346" id="l346">   346</a>                 <span class="k">if</span> <span class="nb">isinstance</span><span class="p">(</span><span class="n">population</span><span class="p">,</span> <span class="nb">list</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l347" id="l347">   347</a>                     <span class="k">raise</span></div>
<div class="parity1 source"><a href="#l348" id="l348">   348</a>                 <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">sample</span><span class="p">(</span><span class="nb">tuple</span><span class="p">(</span><span class="n">population</span><span class="p">),</span> <span class="n">k</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l349" id="l349">   349</a>         <span class="k">return</span> <span class="n">result</span></div>
<div class="parity1 source"><a href="#l350" id="l350">   350</a> </div>
<div class="parity0 source"><a href="#l351" id="l351">   351</a> <span class="c">## -------------------- real-valued distributions  -------------------</span></div>
<div class="parity1 source"><a href="#l352" id="l352">   352</a> </div>
<div class="parity0 source"><a href="#l353" id="l353">   353</a> <span class="c">## -------------------- uniform distribution -------------------</span></div>
<div class="parity1 source"><a href="#l354" id="l354">   354</a> </div>
<div class="parity0 source"><a href="#l355" id="l355">   355</a>     <span class="k">def</span> <span class="nf">uniform</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l356" id="l356">   356</a>         <span class="s">&quot;Get a random number in the range [a, b) or [a, b] depending on rounding.&quot;</span></div>
<div class="parity0 source"><a href="#l357" id="l357">   357</a>         <span class="k">return</span> <span class="n">a</span> <span class="o">+</span> <span class="p">(</span><span class="n">b</span><span class="o">-</span><span class="n">a</span><span class="p">)</span> <span class="o">*</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l358" id="l358">   358</a> </div>
<div class="parity0 source"><a href="#l359" id="l359">   359</a> <span class="c">## -------------------- triangular --------------------</span></div>
<div class="parity1 source"><a href="#l360" id="l360">   360</a> </div>
<div class="parity0 source"><a href="#l361" id="l361">   361</a>     <span class="k">def</span> <span class="nf">triangular</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">low</span><span class="o">=</span><span class="mf">0.0</span><span class="p">,</span> <span class="n">high</span><span class="o">=</span><span class="mf">1.0</span><span class="p">,</span> <span class="n">mode</span><span class="o">=</span><span class="bp">None</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l362" id="l362">   362</a>         <span class="sd">&quot;&quot;&quot;Triangular distribution.</span></div>
<div class="parity0 source"><a href="#l363" id="l363">   363</a> </div>
<div class="parity1 source"><a href="#l364" id="l364">   364</a> <span class="sd">        Continuous distribution bounded by given lower and upper limits,</span></div>
<div class="parity0 source"><a href="#l365" id="l365">   365</a> <span class="sd">        and having a given mode value in-between.</span></div>
<div class="parity1 source"><a href="#l366" id="l366">   366</a> </div>
<div class="parity0 source"><a href="#l367" id="l367">   367</a> <span class="sd">        http://en.wikipedia.org/wiki/Triangular_distribution</span></div>
<div class="parity1 source"><a href="#l368" id="l368">   368</a> </div>
<div class="parity0 source"><a href="#l369" id="l369">   369</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l370" id="l370">   370</a>         <span class="n">u</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l371" id="l371">   371</a>         <span class="n">c</span> <span class="o">=</span> <span class="mf">0.5</span> <span class="k">if</span> <span class="n">mode</span> <span class="ow">is</span> <span class="bp">None</span> <span class="k">else</span> <span class="p">(</span><span class="n">mode</span> <span class="o">-</span> <span class="n">low</span><span class="p">)</span> <span class="o">/</span> <span class="p">(</span><span class="n">high</span> <span class="o">-</span> <span class="n">low</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l372" id="l372">   372</a>         <span class="k">if</span> <span class="n">u</span> <span class="o">&gt;</span> <span class="n">c</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l373" id="l373">   373</a>             <span class="n">u</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="n">u</span></div>
<div class="parity1 source"><a href="#l374" id="l374">   374</a>             <span class="n">c</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="n">c</span></div>
<div class="parity0 source"><a href="#l375" id="l375">   375</a>             <span class="n">low</span><span class="p">,</span> <span class="n">high</span> <span class="o">=</span> <span class="n">high</span><span class="p">,</span> <span class="n">low</span></div>
<div class="parity1 source"><a href="#l376" id="l376">   376</a>         <span class="k">return</span> <span class="n">low</span> <span class="o">+</span> <span class="p">(</span><span class="n">high</span> <span class="o">-</span> <span class="n">low</span><span class="p">)</span> <span class="o">*</span> <span class="p">(</span><span class="n">u</span> <span class="o">*</span> <span class="n">c</span><span class="p">)</span> <span class="o">**</span> <span class="mf">0.5</span></div>
<div class="parity0 source"><a href="#l377" id="l377">   377</a> </div>
<div class="parity1 source"><a href="#l378" id="l378">   378</a> <span class="c">## -------------------- normal distribution --------------------</span></div>
<div class="parity0 source"><a href="#l379" id="l379">   379</a> </div>
<div class="parity1 source"><a href="#l380" id="l380">   380</a>     <span class="k">def</span> <span class="nf">normalvariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">mu</span><span class="p">,</span> <span class="n">sigma</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l381" id="l381">   381</a>         <span class="sd">&quot;&quot;&quot;Normal distribution.</span></div>
<div class="parity1 source"><a href="#l382" id="l382">   382</a> </div>
<div class="parity0 source"><a href="#l383" id="l383">   383</a> <span class="sd">        mu is the mean, and sigma is the standard deviation.</span></div>
<div class="parity1 source"><a href="#l384" id="l384">   384</a> </div>
<div class="parity0 source"><a href="#l385" id="l385">   385</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l386" id="l386">   386</a>         <span class="c"># mu = mean, sigma = standard deviation</span></div>
<div class="parity0 source"><a href="#l387" id="l387">   387</a> </div>
<div class="parity1 source"><a href="#l388" id="l388">   388</a>         <span class="c"># Uses Kinderman and Monahan method. Reference: Kinderman,</span></div>
<div class="parity0 source"><a href="#l389" id="l389">   389</a>         <span class="c"># A.J. and Monahan, J.F., &quot;Computer generation of random</span></div>
<div class="parity1 source"><a href="#l390" id="l390">   390</a>         <span class="c"># variables using the ratio of uniform deviates&quot;, ACM Trans</span></div>
<div class="parity0 source"><a href="#l391" id="l391">   391</a>         <span class="c"># Math Software, 3, (1977), pp257-260.</span></div>
<div class="parity1 source"><a href="#l392" id="l392">   392</a> </div>
<div class="parity0 source"><a href="#l393" id="l393">   393</a>         <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity1 source"><a href="#l394" id="l394">   394</a>         <span class="k">while</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l395" id="l395">   395</a>             <span class="n">u1</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l396" id="l396">   396</a>             <span class="n">u2</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l397" id="l397">   397</a>             <span class="n">z</span> <span class="o">=</span> <span class="n">NV_MAGICCONST</span><span class="o">*</span><span class="p">(</span><span class="n">u1</span><span class="o">-</span><span class="mf">0.5</span><span class="p">)</span><span class="o">/</span><span class="n">u2</span></div>
<div class="parity1 source"><a href="#l398" id="l398">   398</a>             <span class="n">zz</span> <span class="o">=</span> <span class="n">z</span><span class="o">*</span><span class="n">z</span><span class="o">/</span><span class="mf">4.0</span></div>
<div class="parity0 source"><a href="#l399" id="l399">   399</a>             <span class="k">if</span> <span class="n">zz</span> <span class="o">&lt;=</span> <span class="o">-</span><span class="n">_log</span><span class="p">(</span><span class="n">u2</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l400" id="l400">   400</a>                 <span class="k">break</span></div>
<div class="parity0 source"><a href="#l401" id="l401">   401</a>         <span class="k">return</span> <span class="n">mu</span> <span class="o">+</span> <span class="n">z</span><span class="o">*</span><span class="n">sigma</span></div>
<div class="parity1 source"><a href="#l402" id="l402">   402</a> </div>
<div class="parity0 source"><a href="#l403" id="l403">   403</a> <span class="c">## -------------------- lognormal distribution --------------------</span></div>
<div class="parity1 source"><a href="#l404" id="l404">   404</a> </div>
<div class="parity0 source"><a href="#l405" id="l405">   405</a>     <span class="k">def</span> <span class="nf">lognormvariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">mu</span><span class="p">,</span> <span class="n">sigma</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l406" id="l406">   406</a>         <span class="sd">&quot;&quot;&quot;Log normal distribution.</span></div>
<div class="parity0 source"><a href="#l407" id="l407">   407</a> </div>
<div class="parity1 source"><a href="#l408" id="l408">   408</a> <span class="sd">        If you take the natural logarithm of this distribution, you&#39;ll get a</span></div>
<div class="parity0 source"><a href="#l409" id="l409">   409</a> <span class="sd">        normal distribution with mean mu and standard deviation sigma.</span></div>
<div class="parity1 source"><a href="#l410" id="l410">   410</a> <span class="sd">        mu can have any value, and sigma must be greater than zero.</span></div>
<div class="parity0 source"><a href="#l411" id="l411">   411</a> </div>
<div class="parity1 source"><a href="#l412" id="l412">   412</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l413" id="l413">   413</a>         <span class="k">return</span> <span class="n">_exp</span><span class="p">(</span><span class="bp">self</span><span class="o">.</span><span class="n">normalvariate</span><span class="p">(</span><span class="n">mu</span><span class="p">,</span> <span class="n">sigma</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l414" id="l414">   414</a> </div>
<div class="parity0 source"><a href="#l415" id="l415">   415</a> <span class="c">## -------------------- exponential distribution --------------------</span></div>
<div class="parity1 source"><a href="#l416" id="l416">   416</a> </div>
<div class="parity0 source"><a href="#l417" id="l417">   417</a>     <span class="k">def</span> <span class="nf">expovariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">lambd</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l418" id="l418">   418</a>         <span class="sd">&quot;&quot;&quot;Exponential distribution.</span></div>
<div class="parity0 source"><a href="#l419" id="l419">   419</a> </div>
<div class="parity1 source"><a href="#l420" id="l420">   420</a> <span class="sd">        lambd is 1.0 divided by the desired mean.  It should be</span></div>
<div class="parity0 source"><a href="#l421" id="l421">   421</a> <span class="sd">        nonzero.  (The parameter would be called &quot;lambda&quot;, but that is</span></div>
<div class="parity1 source"><a href="#l422" id="l422">   422</a> <span class="sd">        a reserved word in Python.)  Returned values range from 0 to</span></div>
<div class="parity0 source"><a href="#l423" id="l423">   423</a> <span class="sd">        positive infinity if lambd is positive, and from negative</span></div>
<div class="parity1 source"><a href="#l424" id="l424">   424</a> <span class="sd">        infinity to 0 if lambd is negative.</span></div>
<div class="parity0 source"><a href="#l425" id="l425">   425</a> </div>
<div class="parity1 source"><a href="#l426" id="l426">   426</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l427" id="l427">   427</a>         <span class="c"># lambd: rate lambd = 1/mean</span></div>
<div class="parity1 source"><a href="#l428" id="l428">   428</a>         <span class="c"># (&#39;lambda&#39; is a Python reserved word)</span></div>
<div class="parity0 source"><a href="#l429" id="l429">   429</a> </div>
<div class="parity1 source"><a href="#l430" id="l430">   430</a>         <span class="c"># we use 1-random() instead of random() to preclude the</span></div>
<div class="parity0 source"><a href="#l431" id="l431">   431</a>         <span class="c"># possibility of taking the log of zero.</span></div>
<div class="parity1 source"><a href="#l432" id="l432">   432</a>         <span class="k">return</span> <span class="o">-</span><span class="n">_log</span><span class="p">(</span><span class="mf">1.0</span> <span class="o">-</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">())</span><span class="o">/</span><span class="n">lambd</span></div>
<div class="parity0 source"><a href="#l433" id="l433">   433</a> </div>
<div class="parity1 source"><a href="#l434" id="l434">   434</a> <span class="c">## -------------------- von Mises distribution --------------------</span></div>
<div class="parity0 source"><a href="#l435" id="l435">   435</a> </div>
<div class="parity1 source"><a href="#l436" id="l436">   436</a>     <span class="k">def</span> <span class="nf">vonmisesvariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">mu</span><span class="p">,</span> <span class="n">kappa</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l437" id="l437">   437</a>         <span class="sd">&quot;&quot;&quot;Circular data distribution.</span></div>
<div class="parity1 source"><a href="#l438" id="l438">   438</a> </div>
<div class="parity0 source"><a href="#l439" id="l439">   439</a> <span class="sd">        mu is the mean angle, expressed in radians between 0 and 2*pi, and</span></div>
<div class="parity1 source"><a href="#l440" id="l440">   440</a> <span class="sd">        kappa is the concentration parameter, which must be greater than or</span></div>
<div class="parity0 source"><a href="#l441" id="l441">   441</a> <span class="sd">        equal to zero.  If kappa is equal to zero, this distribution reduces</span></div>
<div class="parity1 source"><a href="#l442" id="l442">   442</a> <span class="sd">        to a uniform random angle over the range 0 to 2*pi.</span></div>
<div class="parity0 source"><a href="#l443" id="l443">   443</a> </div>
<div class="parity1 source"><a href="#l444" id="l444">   444</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l445" id="l445">   445</a>         <span class="c"># mu:    mean angle (in radians between 0 and 2*pi)</span></div>
<div class="parity1 source"><a href="#l446" id="l446">   446</a>         <span class="c"># kappa: concentration parameter kappa (&gt;= 0)</span></div>
<div class="parity0 source"><a href="#l447" id="l447">   447</a>         <span class="c"># if kappa = 0 generate uniform random angle</span></div>
<div class="parity1 source"><a href="#l448" id="l448">   448</a> </div>
<div class="parity0 source"><a href="#l449" id="l449">   449</a>         <span class="c"># Based upon an algorithm published in: Fisher, N.I.,</span></div>
<div class="parity1 source"><a href="#l450" id="l450">   450</a>         <span class="c"># &quot;Statistical Analysis of Circular Data&quot;, Cambridge</span></div>
<div class="parity0 source"><a href="#l451" id="l451">   451</a>         <span class="c"># University Press, 1993.</span></div>
<div class="parity1 source"><a href="#l452" id="l452">   452</a> </div>
<div class="parity0 source"><a href="#l453" id="l453">   453</a>         <span class="c"># Thanks to Magnus Kessler for a correction to the</span></div>
<div class="parity1 source"><a href="#l454" id="l454">   454</a>         <span class="c"># implementation of step 4.</span></div>
<div class="parity0 source"><a href="#l455" id="l455">   455</a> </div>
<div class="parity1 source"><a href="#l456" id="l456">   456</a>         <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity0 source"><a href="#l457" id="l457">   457</a>         <span class="k">if</span> <span class="n">kappa</span> <span class="o">&lt;=</span> <span class="mf">1e-6</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l458" id="l458">   458</a>             <span class="k">return</span> <span class="n">TWOPI</span> <span class="o">*</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l459" id="l459">   459</a> </div>
<div class="parity1 source"><a href="#l460" id="l460">   460</a>         <span class="n">a</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">+</span> <span class="n">_sqrt</span><span class="p">(</span><span class="mf">1.0</span> <span class="o">+</span> <span class="mf">4.0</span> <span class="o">*</span> <span class="n">kappa</span> <span class="o">*</span> <span class="n">kappa</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l461" id="l461">   461</a>         <span class="n">b</span> <span class="o">=</span> <span class="p">(</span><span class="n">a</span> <span class="o">-</span> <span class="n">_sqrt</span><span class="p">(</span><span class="mf">2.0</span> <span class="o">*</span> <span class="n">a</span><span class="p">))</span><span class="o">/</span><span class="p">(</span><span class="mf">2.0</span> <span class="o">*</span> <span class="n">kappa</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l462" id="l462">   462</a>         <span class="n">r</span> <span class="o">=</span> <span class="p">(</span><span class="mf">1.0</span> <span class="o">+</span> <span class="n">b</span> <span class="o">*</span> <span class="n">b</span><span class="p">)</span><span class="o">/</span><span class="p">(</span><span class="mf">2.0</span> <span class="o">*</span> <span class="n">b</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l463" id="l463">   463</a> </div>
<div class="parity1 source"><a href="#l464" id="l464">   464</a>         <span class="k">while</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l465" id="l465">   465</a>             <span class="n">u1</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l466" id="l466">   466</a> </div>
<div class="parity0 source"><a href="#l467" id="l467">   467</a>             <span class="n">z</span> <span class="o">=</span> <span class="n">_cos</span><span class="p">(</span><span class="n">_pi</span> <span class="o">*</span> <span class="n">u1</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l468" id="l468">   468</a>             <span class="n">f</span> <span class="o">=</span> <span class="p">(</span><span class="mf">1.0</span> <span class="o">+</span> <span class="n">r</span> <span class="o">*</span> <span class="n">z</span><span class="p">)</span><span class="o">/</span><span class="p">(</span><span class="n">r</span> <span class="o">+</span> <span class="n">z</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l469" id="l469">   469</a>             <span class="n">c</span> <span class="o">=</span> <span class="n">kappa</span> <span class="o">*</span> <span class="p">(</span><span class="n">r</span> <span class="o">-</span> <span class="n">f</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l470" id="l470">   470</a> </div>
<div class="parity0 source"><a href="#l471" id="l471">   471</a>             <span class="n">u2</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l472" id="l472">   472</a> </div>
<div class="parity0 source"><a href="#l473" id="l473">   473</a>             <span class="k">if</span> <span class="n">u2</span> <span class="o">&lt;</span> <span class="n">c</span> <span class="o">*</span> <span class="p">(</span><span class="mf">2.0</span> <span class="o">-</span> <span class="n">c</span><span class="p">)</span> <span class="ow">or</span> <span class="n">u2</span> <span class="o">&lt;=</span> <span class="n">c</span> <span class="o">*</span> <span class="n">_exp</span><span class="p">(</span><span class="mf">1.0</span> <span class="o">-</span> <span class="n">c</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l474" id="l474">   474</a>                 <span class="k">break</span></div>
<div class="parity0 source"><a href="#l475" id="l475">   475</a> </div>
<div class="parity1 source"><a href="#l476" id="l476">   476</a>         <span class="n">u3</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l477" id="l477">   477</a>         <span class="k">if</span> <span class="n">u3</span> <span class="o">&gt;</span> <span class="mf">0.5</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l478" id="l478">   478</a>             <span class="n">theta</span> <span class="o">=</span> <span class="p">(</span><span class="n">mu</span> <span class="o">%</span> <span class="n">TWOPI</span><span class="p">)</span> <span class="o">+</span> <span class="n">_acos</span><span class="p">(</span><span class="n">f</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l479" id="l479">   479</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l480" id="l480">   480</a>             <span class="n">theta</span> <span class="o">=</span> <span class="p">(</span><span class="n">mu</span> <span class="o">%</span> <span class="n">TWOPI</span><span class="p">)</span> <span class="o">-</span> <span class="n">_acos</span><span class="p">(</span><span class="n">f</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l481" id="l481">   481</a> </div>
<div class="parity1 source"><a href="#l482" id="l482">   482</a>         <span class="k">return</span> <span class="n">theta</span></div>
<div class="parity0 source"><a href="#l483" id="l483">   483</a> </div>
<div class="parity1 source"><a href="#l484" id="l484">   484</a> <span class="c">## -------------------- gamma distribution --------------------</span></div>
<div class="parity0 source"><a href="#l485" id="l485">   485</a> </div>
<div class="parity1 source"><a href="#l486" id="l486">   486</a>     <span class="k">def</span> <span class="nf">gammavariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">alpha</span><span class="p">,</span> <span class="n">beta</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l487" id="l487">   487</a>         <span class="sd">&quot;&quot;&quot;Gamma distribution.  Not the gamma function!</span></div>
<div class="parity1 source"><a href="#l488" id="l488">   488</a> </div>
<div class="parity0 source"><a href="#l489" id="l489">   489</a> <span class="sd">        Conditions on the parameters are alpha &gt; 0 and beta &gt; 0.</span></div>
<div class="parity1 source"><a href="#l490" id="l490">   490</a> </div>
<div class="parity0 source"><a href="#l491" id="l491">   491</a> <span class="sd">        The probability distribution function is:</span></div>
<div class="parity1 source"><a href="#l492" id="l492">   492</a> </div>
<div class="parity0 source"><a href="#l493" id="l493">   493</a> <span class="sd">                    x ** (alpha - 1) * math.exp(-x / beta)</span></div>
<div class="parity1 source"><a href="#l494" id="l494">   494</a> <span class="sd">          pdf(x) =  --------------------------------------</span></div>
<div class="parity0 source"><a href="#l495" id="l495">   495</a> <span class="sd">                      math.gamma(alpha) * beta ** alpha</span></div>
<div class="parity1 source"><a href="#l496" id="l496">   496</a> </div>
<div class="parity0 source"><a href="#l497" id="l497">   497</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l498" id="l498">   498</a> </div>
<div class="parity0 source"><a href="#l499" id="l499">   499</a>         <span class="c"># alpha &gt; 0, beta &gt; 0, mean is alpha*beta, variance is alpha*beta**2</span></div>
<div class="parity1 source"><a href="#l500" id="l500">   500</a> </div>
<div class="parity0 source"><a href="#l501" id="l501">   501</a>         <span class="c"># Warning: a few older sources define the gamma distribution in terms</span></div>
<div class="parity1 source"><a href="#l502" id="l502">   502</a>         <span class="c"># of alpha &gt; -1.0</span></div>
<div class="parity0 source"><a href="#l503" id="l503">   503</a>         <span class="k">if</span> <span class="n">alpha</span> <span class="o">&lt;=</span> <span class="mf">0.0</span> <span class="ow">or</span> <span class="n">beta</span> <span class="o">&lt;=</span> <span class="mf">0.0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l504" id="l504">   504</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">,</span> <span class="s">&#39;gammavariate: alpha and beta must be &gt; 0.0&#39;</span></div>
<div class="parity0 source"><a href="#l505" id="l505">   505</a> </div>
<div class="parity1 source"><a href="#l506" id="l506">   506</a>         <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity0 source"><a href="#l507" id="l507">   507</a>         <span class="k">if</span> <span class="n">alpha</span> <span class="o">&gt;</span> <span class="mf">1.0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l508" id="l508">   508</a> </div>
<div class="parity0 source"><a href="#l509" id="l509">   509</a>             <span class="c"># Uses R.C.H. Cheng, &quot;The generation of Gamma</span></div>
<div class="parity1 source"><a href="#l510" id="l510">   510</a>             <span class="c"># variables with non-integral shape parameters&quot;,</span></div>
<div class="parity0 source"><a href="#l511" id="l511">   511</a>             <span class="c"># Applied Statistics, (1977), 26, No. 1, p71-74</span></div>
<div class="parity1 source"><a href="#l512" id="l512">   512</a> </div>
<div class="parity0 source"><a href="#l513" id="l513">   513</a>             <span class="n">ainv</span> <span class="o">=</span> <span class="n">_sqrt</span><span class="p">(</span><span class="mf">2.0</span> <span class="o">*</span> <span class="n">alpha</span> <span class="o">-</span> <span class="mf">1.0</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l514" id="l514">   514</a>             <span class="n">bbb</span> <span class="o">=</span> <span class="n">alpha</span> <span class="o">-</span> <span class="n">LOG4</span></div>
<div class="parity0 source"><a href="#l515" id="l515">   515</a>             <span class="n">ccc</span> <span class="o">=</span> <span class="n">alpha</span> <span class="o">+</span> <span class="n">ainv</span></div>
<div class="parity1 source"><a href="#l516" id="l516">   516</a> </div>
<div class="parity0 source"><a href="#l517" id="l517">   517</a>             <span class="k">while</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l518" id="l518">   518</a>                 <span class="n">u1</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l519" id="l519">   519</a>                 <span class="k">if</span> <span class="ow">not</span> <span class="mf">1e-7</span> <span class="o">&lt;</span> <span class="n">u1</span> <span class="o">&lt;</span> <span class="o">.</span><span class="mi">9999999</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l520" id="l520">   520</a>                     <span class="k">continue</span></div>
<div class="parity0 source"><a href="#l521" id="l521">   521</a>                 <span class="n">u2</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l522" id="l522">   522</a>                 <span class="n">v</span> <span class="o">=</span> <span class="n">_log</span><span class="p">(</span><span class="n">u1</span><span class="o">/</span><span class="p">(</span><span class="mf">1.0</span><span class="o">-</span><span class="n">u1</span><span class="p">))</span><span class="o">/</span><span class="n">ainv</span></div>
<div class="parity0 source"><a href="#l523" id="l523">   523</a>                 <span class="n">x</span> <span class="o">=</span> <span class="n">alpha</span><span class="o">*</span><span class="n">_exp</span><span class="p">(</span><span class="n">v</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l524" id="l524">   524</a>                 <span class="n">z</span> <span class="o">=</span> <span class="n">u1</span><span class="o">*</span><span class="n">u1</span><span class="o">*</span><span class="n">u2</span></div>
<div class="parity0 source"><a href="#l525" id="l525">   525</a>                 <span class="n">r</span> <span class="o">=</span> <span class="n">bbb</span><span class="o">+</span><span class="n">ccc</span><span class="o">*</span><span class="n">v</span><span class="o">-</span><span class="n">x</span></div>
<div class="parity1 source"><a href="#l526" id="l526">   526</a>                 <span class="k">if</span> <span class="n">r</span> <span class="o">+</span> <span class="n">SG_MAGICCONST</span> <span class="o">-</span> <span class="mf">4.5</span><span class="o">*</span><span class="n">z</span> <span class="o">&gt;=</span> <span class="mf">0.0</span> <span class="ow">or</span> <span class="n">r</span> <span class="o">&gt;=</span> <span class="n">_log</span><span class="p">(</span><span class="n">z</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l527" id="l527">   527</a>                     <span class="k">return</span> <span class="n">x</span> <span class="o">*</span> <span class="n">beta</span></div>
<div class="parity1 source"><a href="#l528" id="l528">   528</a> </div>
<div class="parity0 source"><a href="#l529" id="l529">   529</a>         <span class="k">elif</span> <span class="n">alpha</span> <span class="o">==</span> <span class="mf">1.0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l530" id="l530">   530</a>             <span class="c"># expovariate(1)</span></div>
<div class="parity0 source"><a href="#l531" id="l531">   531</a>             <span class="n">u</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l532" id="l532">   532</a>             <span class="k">while</span> <span class="n">u</span> <span class="o">&lt;=</span> <span class="mf">1e-7</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l533" id="l533">   533</a>                 <span class="n">u</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l534" id="l534">   534</a>             <span class="k">return</span> <span class="o">-</span><span class="n">_log</span><span class="p">(</span><span class="n">u</span><span class="p">)</span> <span class="o">*</span> <span class="n">beta</span></div>
<div class="parity0 source"><a href="#l535" id="l535">   535</a> </div>
<div class="parity1 source"><a href="#l536" id="l536">   536</a>         <span class="k">else</span><span class="p">:</span>   <span class="c"># alpha is between 0 and 1 (exclusive)</span></div>
<div class="parity0 source"><a href="#l537" id="l537">   537</a> </div>
<div class="parity1 source"><a href="#l538" id="l538">   538</a>             <span class="c"># Uses ALGORITHM GS of Statistical Computing - Kennedy &amp; Gentle</span></div>
<div class="parity0 source"><a href="#l539" id="l539">   539</a> </div>
<div class="parity1 source"><a href="#l540" id="l540">   540</a>             <span class="k">while</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l541" id="l541">   541</a>                 <span class="n">u</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l542" id="l542">   542</a>                 <span class="n">b</span> <span class="o">=</span> <span class="p">(</span><span class="n">_e</span> <span class="o">+</span> <span class="n">alpha</span><span class="p">)</span><span class="o">/</span><span class="n">_e</span></div>
<div class="parity0 source"><a href="#l543" id="l543">   543</a>                 <span class="n">p</span> <span class="o">=</span> <span class="n">b</span><span class="o">*</span><span class="n">u</span></div>
<div class="parity1 source"><a href="#l544" id="l544">   544</a>                 <span class="k">if</span> <span class="n">p</span> <span class="o">&lt;=</span> <span class="mf">1.0</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l545" id="l545">   545</a>                     <span class="n">x</span> <span class="o">=</span> <span class="n">p</span> <span class="o">**</span> <span class="p">(</span><span class="mf">1.0</span><span class="o">/</span><span class="n">alpha</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l546" id="l546">   546</a>                 <span class="k">else</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l547" id="l547">   547</a>                     <span class="n">x</span> <span class="o">=</span> <span class="o">-</span><span class="n">_log</span><span class="p">((</span><span class="n">b</span><span class="o">-</span><span class="n">p</span><span class="p">)</span><span class="o">/</span><span class="n">alpha</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l548" id="l548">   548</a>                 <span class="n">u1</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l549" id="l549">   549</a>                 <span class="k">if</span> <span class="n">p</span> <span class="o">&gt;</span> <span class="mf">1.0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l550" id="l550">   550</a>                     <span class="k">if</span> <span class="n">u1</span> <span class="o">&lt;=</span> <span class="n">x</span> <span class="o">**</span> <span class="p">(</span><span class="n">alpha</span> <span class="o">-</span> <span class="mf">1.0</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l551" id="l551">   551</a>                         <span class="k">break</span></div>
<div class="parity1 source"><a href="#l552" id="l552">   552</a>                 <span class="k">elif</span> <span class="n">u1</span> <span class="o">&lt;=</span> <span class="n">_exp</span><span class="p">(</span><span class="o">-</span><span class="n">x</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l553" id="l553">   553</a>                     <span class="k">break</span></div>
<div class="parity1 source"><a href="#l554" id="l554">   554</a>             <span class="k">return</span> <span class="n">x</span> <span class="o">*</span> <span class="n">beta</span></div>
<div class="parity0 source"><a href="#l555" id="l555">   555</a> </div>
<div class="parity1 source"><a href="#l556" id="l556">   556</a> <span class="c">## -------------------- Gauss (faster alternative) --------------------</span></div>
<div class="parity0 source"><a href="#l557" id="l557">   557</a> </div>
<div class="parity1 source"><a href="#l558" id="l558">   558</a>     <span class="k">def</span> <span class="nf">gauss</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">mu</span><span class="p">,</span> <span class="n">sigma</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l559" id="l559">   559</a>         <span class="sd">&quot;&quot;&quot;Gaussian distribution.</span></div>
<div class="parity1 source"><a href="#l560" id="l560">   560</a> </div>
<div class="parity0 source"><a href="#l561" id="l561">   561</a> <span class="sd">        mu is the mean, and sigma is the standard deviation.  This is</span></div>
<div class="parity1 source"><a href="#l562" id="l562">   562</a> <span class="sd">        slightly faster than the normalvariate() function.</span></div>
<div class="parity0 source"><a href="#l563" id="l563">   563</a> </div>
<div class="parity1 source"><a href="#l564" id="l564">   564</a> <span class="sd">        Not thread-safe without a lock around calls.</span></div>
<div class="parity0 source"><a href="#l565" id="l565">   565</a> </div>
<div class="parity1 source"><a href="#l566" id="l566">   566</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l567" id="l567">   567</a> </div>
<div class="parity1 source"><a href="#l568" id="l568">   568</a>         <span class="c"># When x and y are two variables from [0, 1), uniformly</span></div>
<div class="parity0 source"><a href="#l569" id="l569">   569</a>         <span class="c"># distributed, then</span></div>
<div class="parity1 source"><a href="#l570" id="l570">   570</a>         <span class="c">#</span></div>
<div class="parity0 source"><a href="#l571" id="l571">   571</a>         <span class="c">#    cos(2*pi*x)*sqrt(-2*log(1-y))</span></div>
<div class="parity1 source"><a href="#l572" id="l572">   572</a>         <span class="c">#    sin(2*pi*x)*sqrt(-2*log(1-y))</span></div>
<div class="parity0 source"><a href="#l573" id="l573">   573</a>         <span class="c">#</span></div>
<div class="parity1 source"><a href="#l574" id="l574">   574</a>         <span class="c"># are two *independent* variables with normal distribution</span></div>
<div class="parity0 source"><a href="#l575" id="l575">   575</a>         <span class="c"># (mu = 0, sigma = 1).</span></div>
<div class="parity1 source"><a href="#l576" id="l576">   576</a>         <span class="c"># (Lambert Meertens)</span></div>
<div class="parity0 source"><a href="#l577" id="l577">   577</a>         <span class="c"># (corrected version; bug discovered by Mike Miller, fixed by LM)</span></div>
<div class="parity1 source"><a href="#l578" id="l578">   578</a> </div>
<div class="parity0 source"><a href="#l579" id="l579">   579</a>         <span class="c"># Multithreading note: When two threads call this function</span></div>
<div class="parity1 source"><a href="#l580" id="l580">   580</a>         <span class="c"># simultaneously, it is possible that they will receive the</span></div>
<div class="parity0 source"><a href="#l581" id="l581">   581</a>         <span class="c"># same return value.  The window is very small though.  To</span></div>
<div class="parity1 source"><a href="#l582" id="l582">   582</a>         <span class="c"># avoid this, you have to use a lock around all calls.  (I</span></div>
<div class="parity0 source"><a href="#l583" id="l583">   583</a>         <span class="c"># didn&#39;t want to slow this down in the serial case by using a</span></div>
<div class="parity1 source"><a href="#l584" id="l584">   584</a>         <span class="c"># lock here.)</span></div>
<div class="parity0 source"><a href="#l585" id="l585">   585</a> </div>
<div class="parity1 source"><a href="#l586" id="l586">   586</a>         <span class="n">random</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity0 source"><a href="#l587" id="l587">   587</a>         <span class="n">z</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span></div>
<div class="parity1 source"><a href="#l588" id="l588">   588</a>         <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="bp">None</span></div>
<div class="parity0 source"><a href="#l589" id="l589">   589</a>         <span class="k">if</span> <span class="n">z</span> <span class="ow">is</span> <span class="bp">None</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l590" id="l590">   590</a>             <span class="n">x2pi</span> <span class="o">=</span> <span class="n">random</span><span class="p">()</span> <span class="o">*</span> <span class="n">TWOPI</span></div>
<div class="parity0 source"><a href="#l591" id="l591">   591</a>             <span class="n">g2rad</span> <span class="o">=</span> <span class="n">_sqrt</span><span class="p">(</span><span class="o">-</span><span class="mf">2.0</span> <span class="o">*</span> <span class="n">_log</span><span class="p">(</span><span class="mf">1.0</span> <span class="o">-</span> <span class="n">random</span><span class="p">()))</span></div>
<div class="parity1 source"><a href="#l592" id="l592">   592</a>             <span class="n">z</span> <span class="o">=</span> <span class="n">_cos</span><span class="p">(</span><span class="n">x2pi</span><span class="p">)</span> <span class="o">*</span> <span class="n">g2rad</span></div>
<div class="parity0 source"><a href="#l593" id="l593">   593</a>             <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="n">_sin</span><span class="p">(</span><span class="n">x2pi</span><span class="p">)</span> <span class="o">*</span> <span class="n">g2rad</span></div>
<div class="parity1 source"><a href="#l594" id="l594">   594</a> </div>
<div class="parity0 source"><a href="#l595" id="l595">   595</a>         <span class="k">return</span> <span class="n">mu</span> <span class="o">+</span> <span class="n">z</span><span class="o">*</span><span class="n">sigma</span></div>
<div class="parity1 source"><a href="#l596" id="l596">   596</a> </div>
<div class="parity0 source"><a href="#l597" id="l597">   597</a> <span class="c">## -------------------- beta --------------------</span></div>
<div class="parity1 source"><a href="#l598" id="l598">   598</a> <span class="c">## See</span></div>
<div class="parity0 source"><a href="#l599" id="l599">   599</a> <span class="c">## http://mail.python.org/pipermail/python-bugs-list/2001-January/003752.html</span></div>
<div class="parity1 source"><a href="#l600" id="l600">   600</a> <span class="c">## for Ivan Frohne&#39;s insightful analysis of why the original implementation:</span></div>
<div class="parity0 source"><a href="#l601" id="l601">   601</a> <span class="c">##</span></div>
<div class="parity1 source"><a href="#l602" id="l602">   602</a> <span class="c">##    def betavariate(self, alpha, beta):</span></div>
<div class="parity0 source"><a href="#l603" id="l603">   603</a> <span class="c">##        # Discrete Event Simulation in C, pp 87-88.</span></div>
<div class="parity1 source"><a href="#l604" id="l604">   604</a> <span class="c">##</span></div>
<div class="parity0 source"><a href="#l605" id="l605">   605</a> <span class="c">##        y = self.expovariate(alpha)</span></div>
<div class="parity1 source"><a href="#l606" id="l606">   606</a> <span class="c">##        z = self.expovariate(1.0/beta)</span></div>
<div class="parity0 source"><a href="#l607" id="l607">   607</a> <span class="c">##        return z/(y+z)</span></div>
<div class="parity1 source"><a href="#l608" id="l608">   608</a> <span class="c">##</span></div>
<div class="parity0 source"><a href="#l609" id="l609">   609</a> <span class="c">## was dead wrong, and how it probably got that way.</span></div>
<div class="parity1 source"><a href="#l610" id="l610">   610</a> </div>
<div class="parity0 source"><a href="#l611" id="l611">   611</a>     <span class="k">def</span> <span class="nf">betavariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">alpha</span><span class="p">,</span> <span class="n">beta</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l612" id="l612">   612</a>         <span class="sd">&quot;&quot;&quot;Beta distribution.</span></div>
<div class="parity0 source"><a href="#l613" id="l613">   613</a> </div>
<div class="parity1 source"><a href="#l614" id="l614">   614</a> <span class="sd">        Conditions on the parameters are alpha &gt; 0 and beta &gt; 0.</span></div>
<div class="parity0 source"><a href="#l615" id="l615">   615</a> <span class="sd">        Returned values range between 0 and 1.</span></div>
<div class="parity1 source"><a href="#l616" id="l616">   616</a> </div>
<div class="parity0 source"><a href="#l617" id="l617">   617</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l618" id="l618">   618</a> </div>
<div class="parity0 source"><a href="#l619" id="l619">   619</a>         <span class="c"># This version due to Janne Sinkkonen, and matches all the std</span></div>
<div class="parity1 source"><a href="#l620" id="l620">   620</a>         <span class="c"># texts (e.g., Knuth Vol 2 Ed 3 pg 134 &quot;the beta distribution&quot;).</span></div>
<div class="parity0 source"><a href="#l621" id="l621">   621</a>         <span class="n">y</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">gammavariate</span><span class="p">(</span><span class="n">alpha</span><span class="p">,</span> <span class="mf">1.</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l622" id="l622">   622</a>         <span class="k">if</span> <span class="n">y</span> <span class="o">==</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l623" id="l623">   623</a>             <span class="k">return</span> <span class="mf">0.0</span></div>
<div class="parity1 source"><a href="#l624" id="l624">   624</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l625" id="l625">   625</a>             <span class="k">return</span> <span class="n">y</span> <span class="o">/</span> <span class="p">(</span><span class="n">y</span> <span class="o">+</span> <span class="bp">self</span><span class="o">.</span><span class="n">gammavariate</span><span class="p">(</span><span class="n">beta</span><span class="p">,</span> <span class="mf">1.</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l626" id="l626">   626</a> </div>
<div class="parity0 source"><a href="#l627" id="l627">   627</a> <span class="c">## -------------------- Pareto --------------------</span></div>
<div class="parity1 source"><a href="#l628" id="l628">   628</a> </div>
<div class="parity0 source"><a href="#l629" id="l629">   629</a>     <span class="k">def</span> <span class="nf">paretovariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">alpha</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l630" id="l630">   630</a>         <span class="sd">&quot;&quot;&quot;Pareto distribution.  alpha is the shape parameter.&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l631" id="l631">   631</a>         <span class="c"># Jain, pg. 495</span></div>
<div class="parity1 source"><a href="#l632" id="l632">   632</a> </div>
<div class="parity0 source"><a href="#l633" id="l633">   633</a>         <span class="n">u</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l634" id="l634">   634</a>         <span class="k">return</span> <span class="mf">1.0</span> <span class="o">/</span> <span class="nb">pow</span><span class="p">(</span><span class="n">u</span><span class="p">,</span> <span class="mf">1.0</span><span class="o">/</span><span class="n">alpha</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l635" id="l635">   635</a> </div>
<div class="parity1 source"><a href="#l636" id="l636">   636</a> <span class="c">## -------------------- Weibull --------------------</span></div>
<div class="parity0 source"><a href="#l637" id="l637">   637</a> </div>
<div class="parity1 source"><a href="#l638" id="l638">   638</a>     <span class="k">def</span> <span class="nf">weibullvariate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">alpha</span><span class="p">,</span> <span class="n">beta</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l639" id="l639">   639</a>         <span class="sd">&quot;&quot;&quot;Weibull distribution.</span></div>
<div class="parity1 source"><a href="#l640" id="l640">   640</a> </div>
<div class="parity0 source"><a href="#l641" id="l641">   641</a> <span class="sd">        alpha is the scale parameter and beta is the shape parameter.</span></div>
<div class="parity1 source"><a href="#l642" id="l642">   642</a> </div>
<div class="parity0 source"><a href="#l643" id="l643">   643</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l644" id="l644">   644</a>         <span class="c"># Jain, pg. 499; bug fix courtesy Bill Arms</span></div>
<div class="parity0 source"><a href="#l645" id="l645">   645</a> </div>
<div class="parity1 source"><a href="#l646" id="l646">   646</a>         <span class="n">u</span> <span class="o">=</span> <span class="mf">1.0</span> <span class="o">-</span> <span class="bp">self</span><span class="o">.</span><span class="n">random</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l647" id="l647">   647</a>         <span class="k">return</span> <span class="n">alpha</span> <span class="o">*</span> <span class="nb">pow</span><span class="p">(</span><span class="o">-</span><span class="n">_log</span><span class="p">(</span><span class="n">u</span><span class="p">),</span> <span class="mf">1.0</span><span class="o">/</span><span class="n">beta</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l648" id="l648">   648</a> </div>
<div class="parity0 source"><a href="#l649" id="l649">   649</a> <span class="c">## -------------------- Wichmann-Hill -------------------</span></div>
<div class="parity1 source"><a href="#l650" id="l650">   650</a> </div>
<div class="parity0 source"><a href="#l651" id="l651">   651</a> <span class="k">class</span> <span class="nc">WichmannHill</span><span class="p">(</span><span class="n">Random</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l652" id="l652">   652</a> </div>
<div class="parity0 source"><a href="#l653" id="l653">   653</a>     <span class="n">VERSION</span> <span class="o">=</span> <span class="mi">1</span>     <span class="c"># used by getstate/setstate</span></div>
<div class="parity1 source"><a href="#l654" id="l654">   654</a> </div>
<div class="parity0 source"><a href="#l655" id="l655">   655</a>     <span class="k">def</span> <span class="nf">seed</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">a</span><span class="o">=</span><span class="bp">None</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l656" id="l656">   656</a>         <span class="sd">&quot;&quot;&quot;Initialize internal state from hashable object.</span></div>
<div class="parity0 source"><a href="#l657" id="l657">   657</a> </div>
<div class="parity1 source"><a href="#l658" id="l658">   658</a> <span class="sd">        None or no argument seeds from current time or from an operating</span></div>
<div class="parity0 source"><a href="#l659" id="l659">   659</a> <span class="sd">        system specific randomness source if available.</span></div>
<div class="parity1 source"><a href="#l660" id="l660">   660</a> </div>
<div class="parity0 source"><a href="#l661" id="l661">   661</a> <span class="sd">        If a is not None or an int or long, hash(a) is used instead.</span></div>
<div class="parity1 source"><a href="#l662" id="l662">   662</a> </div>
<div class="parity0 source"><a href="#l663" id="l663">   663</a> <span class="sd">        If a is an int or long, a is used directly.  Distinct values between</span></div>
<div class="parity1 source"><a href="#l664" id="l664">   664</a> <span class="sd">        0 and 27814431486575L inclusive are guaranteed to yield distinct</span></div>
<div class="parity0 source"><a href="#l665" id="l665">   665</a> <span class="sd">        internal states (this guarantee is specific to the default</span></div>
<div class="parity1 source"><a href="#l666" id="l666">   666</a> <span class="sd">        Wichmann-Hill generator).</span></div>
<div class="parity0 source"><a href="#l667" id="l667">   667</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l668" id="l668">   668</a> </div>
<div class="parity0 source"><a href="#l669" id="l669">   669</a>         <span class="k">if</span> <span class="n">a</span> <span class="ow">is</span> <span class="bp">None</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l670" id="l670">   670</a>             <span class="k">try</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l671" id="l671">   671</a>                 <span class="n">a</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">_hexlify</span><span class="p">(</span><span class="n">_urandom</span><span class="p">(</span><span class="mi">16</span><span class="p">)),</span> <span class="mi">16</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l672" id="l672">   672</a>             <span class="k">except</span> <span class="ne">NotImplementedError</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l673" id="l673">   673</a>                 <span class="kn">import</span> <span class="nn">time</span></div>
<div class="parity1 source"><a href="#l674" id="l674">   674</a>                 <span class="n">a</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">time</span><span class="o">.</span><span class="n">time</span><span class="p">()</span> <span class="o">*</span> <span class="mi">256</span><span class="p">)</span> <span class="c"># use fractional seconds</span></div>
<div class="parity0 source"><a href="#l675" id="l675">   675</a> </div>
<div class="parity1 source"><a href="#l676" id="l676">   676</a>         <span class="k">if</span> <span class="ow">not</span> <span class="nb">isinstance</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="p">(</span><span class="nb">int</span><span class="p">,</span> <span class="nb">long</span><span class="p">)):</span></div>
<div class="parity0 source"><a href="#l677" id="l677">   677</a>             <span class="n">a</span> <span class="o">=</span> <span class="nb">hash</span><span class="p">(</span><span class="n">a</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l678" id="l678">   678</a> </div>
<div class="parity0 source"><a href="#l679" id="l679">   679</a>         <span class="n">a</span><span class="p">,</span> <span class="n">x</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">30268</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l680" id="l680">   680</a>         <span class="n">a</span><span class="p">,</span> <span class="n">y</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">30306</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l681" id="l681">   681</a>         <span class="n">a</span><span class="p">,</span> <span class="n">z</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">30322</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l682" id="l682">   682</a>         <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">x</span><span class="p">)</span><span class="o">+</span><span class="mi">1</span><span class="p">,</span> <span class="nb">int</span><span class="p">(</span><span class="n">y</span><span class="p">)</span><span class="o">+</span><span class="mi">1</span><span class="p">,</span> <span class="nb">int</span><span class="p">(</span><span class="n">z</span><span class="p">)</span><span class="o">+</span><span class="mi">1</span></div>
<div class="parity0 source"><a href="#l683" id="l683">   683</a> </div>
<div class="parity1 source"><a href="#l684" id="l684">   684</a>         <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="bp">None</span></div>
<div class="parity0 source"><a href="#l685" id="l685">   685</a> </div>
<div class="parity1 source"><a href="#l686" id="l686">   686</a>     <span class="k">def</span> <span class="nf">random</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l687" id="l687">   687</a>         <span class="sd">&quot;&quot;&quot;Get the next random number in the range [0.0, 1.0).&quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l688" id="l688">   688</a> </div>
<div class="parity0 source"><a href="#l689" id="l689">   689</a>         <span class="c"># Wichman-Hill random number generator.</span></div>
<div class="parity1 source"><a href="#l690" id="l690">   690</a>         <span class="c">#</span></div>
<div class="parity0 source"><a href="#l691" id="l691">   691</a>         <span class="c"># Wichmann, B. A. &amp; Hill, I. D. (1982)</span></div>
<div class="parity1 source"><a href="#l692" id="l692">   692</a>         <span class="c"># Algorithm AS 183:</span></div>
<div class="parity0 source"><a href="#l693" id="l693">   693</a>         <span class="c"># An efficient and portable pseudo-random number generator</span></div>
<div class="parity1 source"><a href="#l694" id="l694">   694</a>         <span class="c"># Applied Statistics 31 (1982) 188-190</span></div>
<div class="parity0 source"><a href="#l695" id="l695">   695</a>         <span class="c">#</span></div>
<div class="parity1 source"><a href="#l696" id="l696">   696</a>         <span class="c"># see also:</span></div>
<div class="parity0 source"><a href="#l697" id="l697">   697</a>         <span class="c">#        Correction to Algorithm AS 183</span></div>
<div class="parity1 source"><a href="#l698" id="l698">   698</a>         <span class="c">#        Applied Statistics 33 (1984) 123</span></div>
<div class="parity0 source"><a href="#l699" id="l699">   699</a>         <span class="c">#</span></div>
<div class="parity1 source"><a href="#l700" id="l700">   700</a>         <span class="c">#        McLeod, A. I. (1985)</span></div>
<div class="parity0 source"><a href="#l701" id="l701">   701</a>         <span class="c">#        A remark on Algorithm AS 183</span></div>
<div class="parity1 source"><a href="#l702" id="l702">   702</a>         <span class="c">#        Applied Statistics 34 (1985),198-200</span></div>
<div class="parity0 source"><a href="#l703" id="l703">   703</a> </div>
<div class="parity1 source"><a href="#l704" id="l704">   704</a>         <span class="c"># This part is thread-unsafe:</span></div>
<div class="parity0 source"><a href="#l705" id="l705">   705</a>         <span class="c"># BEGIN CRITICAL SECTION</span></div>
<div class="parity1 source"><a href="#l706" id="l706">   706</a>         <span class="n">x</span><span class="p">,</span> <span class="n">y</span><span class="p">,</span> <span class="n">z</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span></div>
<div class="parity0 source"><a href="#l707" id="l707">   707</a>         <span class="n">x</span> <span class="o">=</span> <span class="p">(</span><span class="mi">171</span> <span class="o">*</span> <span class="n">x</span><span class="p">)</span> <span class="o">%</span> <span class="mi">30269</span></div>
<div class="parity1 source"><a href="#l708" id="l708">   708</a>         <span class="n">y</span> <span class="o">=</span> <span class="p">(</span><span class="mi">172</span> <span class="o">*</span> <span class="n">y</span><span class="p">)</span> <span class="o">%</span> <span class="mi">30307</span></div>
<div class="parity0 source"><a href="#l709" id="l709">   709</a>         <span class="n">z</span> <span class="o">=</span> <span class="p">(</span><span class="mi">170</span> <span class="o">*</span> <span class="n">z</span><span class="p">)</span> <span class="o">%</span> <span class="mi">30323</span></div>
<div class="parity1 source"><a href="#l710" id="l710">   710</a>         <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span> <span class="o">=</span> <span class="n">x</span><span class="p">,</span> <span class="n">y</span><span class="p">,</span> <span class="n">z</span></div>
<div class="parity0 source"><a href="#l711" id="l711">   711</a>         <span class="c"># END CRITICAL SECTION</span></div>
<div class="parity1 source"><a href="#l712" id="l712">   712</a> </div>
<div class="parity0 source"><a href="#l713" id="l713">   713</a>         <span class="c"># Note:  on a platform using IEEE-754 double arithmetic, this can</span></div>
<div class="parity1 source"><a href="#l714" id="l714">   714</a>         <span class="c"># never return 0.0 (asserted by Tim; proof too long for a comment).</span></div>
<div class="parity0 source"><a href="#l715" id="l715">   715</a>         <span class="k">return</span> <span class="p">(</span><span class="n">x</span><span class="o">/</span><span class="mf">30269.0</span> <span class="o">+</span> <span class="n">y</span><span class="o">/</span><span class="mf">30307.0</span> <span class="o">+</span> <span class="n">z</span><span class="o">/</span><span class="mf">30323.0</span><span class="p">)</span> <span class="o">%</span> <span class="mf">1.0</span></div>
<div class="parity1 source"><a href="#l716" id="l716">   716</a> </div>
<div class="parity0 source"><a href="#l717" id="l717">   717</a>     <span class="k">def</span> <span class="nf">getstate</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l718" id="l718">   718</a>         <span class="sd">&quot;&quot;&quot;Return internal state; can be passed to setstate() later.&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l719" id="l719">   719</a>         <span class="k">return</span> <span class="bp">self</span><span class="o">.</span><span class="n">VERSION</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span></div>
<div class="parity1 source"><a href="#l720" id="l720">   720</a> </div>
<div class="parity0 source"><a href="#l721" id="l721">   721</a>     <span class="k">def</span> <span class="nf">setstate</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">state</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l722" id="l722">   722</a>         <span class="sd">&quot;&quot;&quot;Restore internal state from object returned by getstate().&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l723" id="l723">   723</a>         <span class="n">version</span> <span class="o">=</span> <span class="n">state</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span></div>
<div class="parity1 source"><a href="#l724" id="l724">   724</a>         <span class="k">if</span> <span class="n">version</span> <span class="o">==</span> <span class="mi">1</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l725" id="l725">   725</a>             <span class="n">version</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="n">state</span></div>
<div class="parity1 source"><a href="#l726" id="l726">   726</a>         <span class="k">else</span><span class="p">:</span></div>
<div class="parity0 source"><a href="#l727" id="l727">   727</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&quot;state with version </span><span class="si">%s</span><span class="s"> passed to &quot;</span></div>
<div class="parity1 source"><a href="#l728" id="l728">   728</a>                              <span class="s">&quot;Random.setstate() of version </span><span class="si">%s</span><span class="s">&quot;</span> <span class="o">%</span></div>
<div class="parity0 source"><a href="#l729" id="l729">   729</a>                              <span class="p">(</span><span class="n">version</span><span class="p">,</span> <span class="bp">self</span><span class="o">.</span><span class="n">VERSION</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l730" id="l730">   730</a> </div>
<div class="parity0 source"><a href="#l731" id="l731">   731</a>     <span class="k">def</span> <span class="nf">jumpahead</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">n</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l732" id="l732">   732</a>         <span class="sd">&quot;&quot;&quot;Act as if n calls to random() were made, but quickly.</span></div>
<div class="parity0 source"><a href="#l733" id="l733">   733</a> </div>
<div class="parity1 source"><a href="#l734" id="l734">   734</a> <span class="sd">        n is an int, greater than or equal to 0.</span></div>
<div class="parity0 source"><a href="#l735" id="l735">   735</a> </div>
<div class="parity1 source"><a href="#l736" id="l736">   736</a> <span class="sd">        Example use:  If you have 2 threads and know that each will</span></div>
<div class="parity0 source"><a href="#l737" id="l737">   737</a> <span class="sd">        consume no more than a million random numbers, create two Random</span></div>
<div class="parity1 source"><a href="#l738" id="l738">   738</a> <span class="sd">        objects r1 and r2, then do</span></div>
<div class="parity0 source"><a href="#l739" id="l739">   739</a> <span class="sd">            r2.setstate(r1.getstate())</span></div>
<div class="parity1 source"><a href="#l740" id="l740">   740</a> <span class="sd">            r2.jumpahead(1000000)</span></div>
<div class="parity0 source"><a href="#l741" id="l741">   741</a> <span class="sd">        Then r1 and r2 will use guaranteed-disjoint segments of the full</span></div>
<div class="parity1 source"><a href="#l742" id="l742">   742</a> <span class="sd">        period.</span></div>
<div class="parity0 source"><a href="#l743" id="l743">   743</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l744" id="l744">   744</a> </div>
<div class="parity0 source"><a href="#l745" id="l745">   745</a>         <span class="k">if</span> <span class="ow">not</span> <span class="n">n</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l746" id="l746">   746</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&quot;n must be &gt;= 0&quot;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l747" id="l747">   747</a>         <span class="n">x</span><span class="p">,</span> <span class="n">y</span><span class="p">,</span> <span class="n">z</span> <span class="o">=</span> <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span></div>
<div class="parity1 source"><a href="#l748" id="l748">   748</a>         <span class="n">x</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">x</span> <span class="o">*</span> <span class="nb">pow</span><span class="p">(</span><span class="mi">171</span><span class="p">,</span> <span class="n">n</span><span class="p">,</span> <span class="mi">30269</span><span class="p">))</span> <span class="o">%</span> <span class="mi">30269</span></div>
<div class="parity0 source"><a href="#l749" id="l749">   749</a>         <span class="n">y</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">y</span> <span class="o">*</span> <span class="nb">pow</span><span class="p">(</span><span class="mi">172</span><span class="p">,</span> <span class="n">n</span><span class="p">,</span> <span class="mi">30307</span><span class="p">))</span> <span class="o">%</span> <span class="mi">30307</span></div>
<div class="parity1 source"><a href="#l750" id="l750">   750</a>         <span class="n">z</span> <span class="o">=</span> <span class="nb">int</span><span class="p">(</span><span class="n">z</span> <span class="o">*</span> <span class="nb">pow</span><span class="p">(</span><span class="mi">170</span><span class="p">,</span> <span class="n">n</span><span class="p">,</span> <span class="mi">30323</span><span class="p">))</span> <span class="o">%</span> <span class="mi">30323</span></div>
<div class="parity0 source"><a href="#l751" id="l751">   751</a>         <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span> <span class="o">=</span> <span class="n">x</span><span class="p">,</span> <span class="n">y</span><span class="p">,</span> <span class="n">z</span></div>
<div class="parity1 source"><a href="#l752" id="l752">   752</a> </div>
<div class="parity0 source"><a href="#l753" id="l753">   753</a>     <span class="k">def</span> <span class="nf">__whseed</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">x</span><span class="o">=</span><span class="mi">0</span><span class="p">,</span> <span class="n">y</span><span class="o">=</span><span class="mi">0</span><span class="p">,</span> <span class="n">z</span><span class="o">=</span><span class="mi">0</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l754" id="l754">   754</a>         <span class="sd">&quot;&quot;&quot;Set the Wichmann-Hill seed from (x, y, z).</span></div>
<div class="parity0 source"><a href="#l755" id="l755">   755</a> </div>
<div class="parity1 source"><a href="#l756" id="l756">   756</a> <span class="sd">        These must be integers in the range [0, 256).</span></div>
<div class="parity0 source"><a href="#l757" id="l757">   757</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l758" id="l758">   758</a> </div>
<div class="parity0 source"><a href="#l759" id="l759">   759</a>         <span class="k">if</span> <span class="ow">not</span> <span class="nb">type</span><span class="p">(</span><span class="n">x</span><span class="p">)</span> <span class="o">==</span> <span class="nb">type</span><span class="p">(</span><span class="n">y</span><span class="p">)</span> <span class="o">==</span> <span class="nb">type</span><span class="p">(</span><span class="n">z</span><span class="p">)</span> <span class="o">==</span> <span class="nb">int</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l760" id="l760">   760</a>             <span class="k">raise</span> <span class="ne">TypeError</span><span class="p">(</span><span class="s">&#39;seeds must be integers&#39;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l761" id="l761">   761</a>         <span class="k">if</span> <span class="ow">not</span> <span class="p">(</span><span class="mi">0</span> <span class="o">&lt;=</span> <span class="n">x</span> <span class="o">&lt;</span> <span class="mi">256</span> <span class="ow">and</span> <span class="mi">0</span> <span class="o">&lt;=</span> <span class="n">y</span> <span class="o">&lt;</span> <span class="mi">256</span> <span class="ow">and</span> <span class="mi">0</span> <span class="o">&lt;=</span> <span class="n">z</span> <span class="o">&lt;</span> <span class="mi">256</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l762" id="l762">   762</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&#39;seeds must be in range(0, 256)&#39;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l763" id="l763">   763</a>         <span class="k">if</span> <span class="mi">0</span> <span class="o">==</span> <span class="n">x</span> <span class="o">==</span> <span class="n">y</span> <span class="o">==</span> <span class="n">z</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l764" id="l764">   764</a>             <span class="c"># Initialize from current time</span></div>
<div class="parity0 source"><a href="#l765" id="l765">   765</a>             <span class="kn">import</span> <span class="nn">time</span></div>
<div class="parity1 source"><a href="#l766" id="l766">   766</a>             <span class="n">t</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">time</span><span class="o">.</span><span class="n">time</span><span class="p">()</span> <span class="o">*</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l767" id="l767">   767</a>             <span class="n">t</span> <span class="o">=</span> <span class="nb">int</span><span class="p">((</span><span class="n">t</span><span class="o">&amp;</span><span class="mh">0xffffff</span><span class="p">)</span> <span class="o">^</span> <span class="p">(</span><span class="n">t</span><span class="o">&gt;&gt;</span><span class="mi">24</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l768" id="l768">   768</a>             <span class="n">t</span><span class="p">,</span> <span class="n">x</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">t</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l769" id="l769">   769</a>             <span class="n">t</span><span class="p">,</span> <span class="n">y</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">t</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l770" id="l770">   770</a>             <span class="n">t</span><span class="p">,</span> <span class="n">z</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">t</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l771" id="l771">   771</a>         <span class="c"># Zero is a poor seed, so substitute 1</span></div>
<div class="parity1 source"><a href="#l772" id="l772">   772</a>         <span class="bp">self</span><span class="o">.</span><span class="n">_seed</span> <span class="o">=</span> <span class="p">(</span><span class="n">x</span> <span class="ow">or</span> <span class="mi">1</span><span class="p">,</span> <span class="n">y</span> <span class="ow">or</span> <span class="mi">1</span><span class="p">,</span> <span class="n">z</span> <span class="ow">or</span> <span class="mi">1</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l773" id="l773">   773</a> </div>
<div class="parity1 source"><a href="#l774" id="l774">   774</a>         <span class="bp">self</span><span class="o">.</span><span class="n">gauss_next</span> <span class="o">=</span> <span class="bp">None</span></div>
<div class="parity0 source"><a href="#l775" id="l775">   775</a> </div>
<div class="parity1 source"><a href="#l776" id="l776">   776</a>     <span class="k">def</span> <span class="nf">whseed</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">a</span><span class="o">=</span><span class="bp">None</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l777" id="l777">   777</a>         <span class="sd">&quot;&quot;&quot;Seed from hashable object&#39;s hash code.</span></div>
<div class="parity1 source"><a href="#l778" id="l778">   778</a> </div>
<div class="parity0 source"><a href="#l779" id="l779">   779</a> <span class="sd">        None or no argument seeds from current time.  It is not guaranteed</span></div>
<div class="parity1 source"><a href="#l780" id="l780">   780</a> <span class="sd">        that objects with distinct hash codes lead to distinct internal</span></div>
<div class="parity0 source"><a href="#l781" id="l781">   781</a> <span class="sd">        states.</span></div>
<div class="parity1 source"><a href="#l782" id="l782">   782</a> </div>
<div class="parity0 source"><a href="#l783" id="l783">   783</a> <span class="sd">        This is obsolete, provided for compatibility with the seed routine</span></div>
<div class="parity1 source"><a href="#l784" id="l784">   784</a> <span class="sd">        used prior to Python 2.1.  Use the .seed() method instead.</span></div>
<div class="parity0 source"><a href="#l785" id="l785">   785</a> <span class="sd">        &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l786" id="l786">   786</a> </div>
<div class="parity0 source"><a href="#l787" id="l787">   787</a>         <span class="k">if</span> <span class="n">a</span> <span class="ow">is</span> <span class="bp">None</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l788" id="l788">   788</a>             <span class="bp">self</span><span class="o">.</span><span class="n">__whseed</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l789" id="l789">   789</a>             <span class="k">return</span></div>
<div class="parity1 source"><a href="#l790" id="l790">   790</a>         <span class="n">a</span> <span class="o">=</span> <span class="nb">hash</span><span class="p">(</span><span class="n">a</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l791" id="l791">   791</a>         <span class="n">a</span><span class="p">,</span> <span class="n">x</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l792" id="l792">   792</a>         <span class="n">a</span><span class="p">,</span> <span class="n">y</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l793" id="l793">   793</a>         <span class="n">a</span><span class="p">,</span> <span class="n">z</span> <span class="o">=</span> <span class="nb">divmod</span><span class="p">(</span><span class="n">a</span><span class="p">,</span> <span class="mi">256</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l794" id="l794">   794</a>         <span class="n">x</span> <span class="o">=</span> <span class="p">(</span><span class="n">x</span> <span class="o">+</span> <span class="n">a</span><span class="p">)</span> <span class="o">%</span> <span class="mi">256</span> <span class="ow">or</span> <span class="mi">1</span></div>
<div class="parity0 source"><a href="#l795" id="l795">   795</a>         <span class="n">y</span> <span class="o">=</span> <span class="p">(</span><span class="n">y</span> <span class="o">+</span> <span class="n">a</span><span class="p">)</span> <span class="o">%</span> <span class="mi">256</span> <span class="ow">or</span> <span class="mi">1</span></div>
<div class="parity1 source"><a href="#l796" id="l796">   796</a>         <span class="n">z</span> <span class="o">=</span> <span class="p">(</span><span class="n">z</span> <span class="o">+</span> <span class="n">a</span><span class="p">)</span> <span class="o">%</span> <span class="mi">256</span> <span class="ow">or</span> <span class="mi">1</span></div>
<div class="parity0 source"><a href="#l797" id="l797">   797</a>         <span class="bp">self</span><span class="o">.</span><span class="n">__whseed</span><span class="p">(</span><span class="n">x</span><span class="p">,</span> <span class="n">y</span><span class="p">,</span> <span class="n">z</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l798" id="l798">   798</a> </div>
<div class="parity0 source"><a href="#l799" id="l799">   799</a> <span class="c">## --------------- Operating System Random Source  ------------------</span></div>
<div class="parity1 source"><a href="#l800" id="l800">   800</a> </div>
<div class="parity0 source"><a href="#l801" id="l801">   801</a> <span class="k">class</span> <span class="nc">SystemRandom</span><span class="p">(</span><span class="n">Random</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l802" id="l802">   802</a>     <span class="sd">&quot;&quot;&quot;Alternate random number generator using sources provided</span></div>
<div class="parity0 source"><a href="#l803" id="l803">   803</a> <span class="sd">    by the operating system (such as /dev/urandom on Unix or</span></div>
<div class="parity1 source"><a href="#l804" id="l804">   804</a> <span class="sd">    CryptGenRandom on Windows).</span></div>
<div class="parity0 source"><a href="#l805" id="l805">   805</a> </div>
<div class="parity1 source"><a href="#l806" id="l806">   806</a> <span class="sd">     Not available on all systems (see os.urandom() for details).</span></div>
<div class="parity0 source"><a href="#l807" id="l807">   807</a> <span class="sd">    &quot;&quot;&quot;</span></div>
<div class="parity1 source"><a href="#l808" id="l808">   808</a> </div>
<div class="parity0 source"><a href="#l809" id="l809">   809</a>     <span class="k">def</span> <span class="nf">random</span><span class="p">(</span><span class="bp">self</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l810" id="l810">   810</a>         <span class="sd">&quot;&quot;&quot;Get the next random number in the range [0.0, 1.0).&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l811" id="l811">   811</a>         <span class="k">return</span> <span class="p">(</span><span class="nb">long</span><span class="p">(</span><span class="n">_hexlify</span><span class="p">(</span><span class="n">_urandom</span><span class="p">(</span><span class="mi">7</span><span class="p">)),</span> <span class="mi">16</span><span class="p">)</span> <span class="o">&gt;&gt;</span> <span class="mi">3</span><span class="p">)</span> <span class="o">*</span> <span class="n">RECIP_BPF</span></div>
<div class="parity1 source"><a href="#l812" id="l812">   812</a> </div>
<div class="parity0 source"><a href="#l813" id="l813">   813</a>     <span class="k">def</span> <span class="nf">getrandbits</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="n">k</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l814" id="l814">   814</a>         <span class="sd">&quot;&quot;&quot;getrandbits(k) -&gt; x.  Generates a long int with k random bits.&quot;&quot;&quot;</span></div>
<div class="parity0 source"><a href="#l815" id="l815">   815</a>         <span class="k">if</span> <span class="n">k</span> <span class="o">&lt;=</span> <span class="mi">0</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l816" id="l816">   816</a>             <span class="k">raise</span> <span class="ne">ValueError</span><span class="p">(</span><span class="s">&#39;number of bits must be greater than zero&#39;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l817" id="l817">   817</a>         <span class="k">if</span> <span class="n">k</span> <span class="o">!=</span> <span class="nb">int</span><span class="p">(</span><span class="n">k</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l818" id="l818">   818</a>             <span class="k">raise</span> <span class="ne">TypeError</span><span class="p">(</span><span class="s">&#39;number of bits should be an integer&#39;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l819" id="l819">   819</a>         <span class="nb">bytes</span> <span class="o">=</span> <span class="p">(</span><span class="n">k</span> <span class="o">+</span> <span class="mi">7</span><span class="p">)</span> <span class="o">//</span> <span class="mi">8</span>                    <span class="c"># bits / 8 and rounded up</span></div>
<div class="parity1 source"><a href="#l820" id="l820">   820</a>         <span class="n">x</span> <span class="o">=</span> <span class="nb">long</span><span class="p">(</span><span class="n">_hexlify</span><span class="p">(</span><span class="n">_urandom</span><span class="p">(</span><span class="nb">bytes</span><span class="p">)),</span> <span class="mi">16</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l821" id="l821">   821</a>         <span class="k">return</span> <span class="n">x</span> <span class="o">&gt;&gt;</span> <span class="p">(</span><span class="nb">bytes</span> <span class="o">*</span> <span class="mi">8</span> <span class="o">-</span> <span class="n">k</span><span class="p">)</span>             <span class="c"># trim excess bits</span></div>
<div class="parity1 source"><a href="#l822" id="l822">   822</a> </div>
<div class="parity0 source"><a href="#l823" id="l823">   823</a>     <span class="k">def</span> <span class="nf">_stub</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="o">*</span><span class="n">args</span><span class="p">,</span> <span class="o">**</span><span class="n">kwds</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l824" id="l824">   824</a>         <span class="s">&quot;Stub method.  Not used for a system random number generator.&quot;</span></div>
<div class="parity0 source"><a href="#l825" id="l825">   825</a>         <span class="k">return</span> <span class="bp">None</span></div>
<div class="parity1 source"><a href="#l826" id="l826">   826</a>     <span class="n">seed</span> <span class="o">=</span> <span class="n">jumpahead</span> <span class="o">=</span> <span class="n">_stub</span></div>
<div class="parity0 source"><a href="#l827" id="l827">   827</a> </div>
<div class="parity1 source"><a href="#l828" id="l828">   828</a>     <span class="k">def</span> <span class="nf">_notimplemented</span><span class="p">(</span><span class="bp">self</span><span class="p">,</span> <span class="o">*</span><span class="n">args</span><span class="p">,</span> <span class="o">**</span><span class="n">kwds</span><span class="p">):</span></div>
<div class="parity0 source"><a href="#l829" id="l829">   829</a>         <span class="s">&quot;Method should not be called for a system random number generator.&quot;</span></div>
<div class="parity1 source"><a href="#l830" id="l830">   830</a>         <span class="k">raise</span> <span class="ne">NotImplementedError</span><span class="p">(</span><span class="s">&#39;System entropy source does not have state.&#39;</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l831" id="l831">   831</a>     <span class="n">getstate</span> <span class="o">=</span> <span class="n">setstate</span> <span class="o">=</span> <span class="n">_notimplemented</span></div>
<div class="parity1 source"><a href="#l832" id="l832">   832</a> </div>
<div class="parity0 source"><a href="#l833" id="l833">   833</a> <span class="c">## -------------------- test program --------------------</span></div>
<div class="parity1 source"><a href="#l834" id="l834">   834</a> </div>
<div class="parity0 source"><a href="#l835" id="l835">   835</a> <span class="k">def</span> <span class="nf">_test_generator</span><span class="p">(</span><span class="n">n</span><span class="p">,</span> <span class="n">func</span><span class="p">,</span> <span class="n">args</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l836" id="l836">   836</a>     <span class="kn">import</span> <span class="nn">time</span></div>
<div class="parity0 source"><a href="#l837" id="l837">   837</a>     <span class="k">print</span> <span class="n">n</span><span class="p">,</span> <span class="s">&#39;times&#39;</span><span class="p">,</span> <span class="n">func</span><span class="o">.</span><span class="n">__name__</span></div>
<div class="parity1 source"><a href="#l838" id="l838">   838</a>     <span class="n">total</span> <span class="o">=</span> <span class="mf">0.0</span></div>
<div class="parity0 source"><a href="#l839" id="l839">   839</a>     <span class="n">sqsum</span> <span class="o">=</span> <span class="mf">0.0</span></div>
<div class="parity1 source"><a href="#l840" id="l840">   840</a>     <span class="n">smallest</span> <span class="o">=</span> <span class="mf">1e10</span></div>
<div class="parity0 source"><a href="#l841" id="l841">   841</a>     <span class="n">largest</span> <span class="o">=</span> <span class="o">-</span><span class="mf">1e10</span></div>
<div class="parity1 source"><a href="#l842" id="l842">   842</a>     <span class="n">t0</span> <span class="o">=</span> <span class="n">time</span><span class="o">.</span><span class="n">time</span><span class="p">()</span></div>
<div class="parity0 source"><a href="#l843" id="l843">   843</a>     <span class="k">for</span> <span class="n">i</span> <span class="ow">in</span> <span class="nb">range</span><span class="p">(</span><span class="n">n</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l844" id="l844">   844</a>         <span class="n">x</span> <span class="o">=</span> <span class="n">func</span><span class="p">(</span><span class="o">*</span><span class="n">args</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l845" id="l845">   845</a>         <span class="n">total</span> <span class="o">+=</span> <span class="n">x</span></div>
<div class="parity1 source"><a href="#l846" id="l846">   846</a>         <span class="n">sqsum</span> <span class="o">=</span> <span class="n">sqsum</span> <span class="o">+</span> <span class="n">x</span><span class="o">*</span><span class="n">x</span></div>
<div class="parity0 source"><a href="#l847" id="l847">   847</a>         <span class="n">smallest</span> <span class="o">=</span> <span class="nb">min</span><span class="p">(</span><span class="n">x</span><span class="p">,</span> <span class="n">smallest</span><span class="p">)</span></div>
<div class="parity1 source"><a href="#l848" id="l848">   848</a>         <span class="n">largest</span> <span class="o">=</span> <span class="nb">max</span><span class="p">(</span><span class="n">x</span><span class="p">,</span> <span class="n">largest</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l849" id="l849">   849</a>     <span class="n">t1</span> <span class="o">=</span> <span class="n">time</span><span class="o">.</span><span class="n">time</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l850" id="l850">   850</a>     <span class="k">print</span> <span class="nb">round</span><span class="p">(</span><span class="n">t1</span><span class="o">-</span><span class="n">t0</span><span class="p">,</span> <span class="mi">3</span><span class="p">),</span> <span class="s">&#39;sec,&#39;</span><span class="p">,</span></div>
<div class="parity0 source"><a href="#l851" id="l851">   851</a>     <span class="n">avg</span> <span class="o">=</span> <span class="n">total</span><span class="o">/</span><span class="n">n</span></div>
<div class="parity1 source"><a href="#l852" id="l852">   852</a>     <span class="n">stddev</span> <span class="o">=</span> <span class="n">_sqrt</span><span class="p">(</span><span class="n">sqsum</span><span class="o">/</span><span class="n">n</span> <span class="o">-</span> <span class="n">avg</span><span class="o">*</span><span class="n">avg</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l853" id="l853">   853</a>     <span class="k">print</span> <span class="s">&#39;avg </span><span class="si">%g</span><span class="s">, stddev </span><span class="si">%g</span><span class="s">, min </span><span class="si">%g</span><span class="s">, max </span><span class="si">%g</span><span class="s">&#39;</span> <span class="o">%</span> \</div>
<div class="parity1 source"><a href="#l854" id="l854">   854</a>               <span class="p">(</span><span class="n">avg</span><span class="p">,</span> <span class="n">stddev</span><span class="p">,</span> <span class="n">smallest</span><span class="p">,</span> <span class="n">largest</span><span class="p">)</span></div>
<div class="parity0 source"><a href="#l855" id="l855">   855</a> </div>
<div class="parity1 source"><a href="#l856" id="l856">   856</a> </div>
<div class="parity0 source"><a href="#l857" id="l857">   857</a> <span class="k">def</span> <span class="nf">_test</span><span class="p">(</span><span class="n">N</span><span class="o">=</span><span class="mi">2000</span><span class="p">):</span></div>
<div class="parity1 source"><a href="#l858" id="l858">   858</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">random</span><span class="p">,</span> <span class="p">())</span></div>
<div class="parity0 source"><a href="#l859" id="l859">   859</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">normalvariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l860" id="l860">   860</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">lognormvariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l861" id="l861">   861</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">vonmisesvariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l862" id="l862">   862</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.01</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l863" id="l863">   863</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.1</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l864" id="l864">   864</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.1</span><span class="p">,</span> <span class="mf">2.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l865" id="l865">   865</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.5</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l866" id="l866">   866</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.9</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l867" id="l867">   867</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">1.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l868" id="l868">   868</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">2.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l869" id="l869">   869</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">20.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l870" id="l870">   870</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gammavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">200.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l871" id="l871">   871</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">gauss</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l872" id="l872">   872</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">betavariate</span><span class="p">,</span> <span class="p">(</span><span class="mf">3.0</span><span class="p">,</span> <span class="mf">3.0</span><span class="p">))</span></div>
<div class="parity0 source"><a href="#l873" id="l873">   873</a>     <span class="n">_test_generator</span><span class="p">(</span><span class="n">N</span><span class="p">,</span> <span class="n">triangular</span><span class="p">,</span> <span class="p">(</span><span class="mf">0.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="p">,</span> <span class="mf">1.0</span><span class="o">/</span><span class="mf">3.0</span><span class="p">))</span></div>
<div class="parity1 source"><a href="#l874" id="l874">   874</a> </div>
<div class="parity0 source"><a href="#l875" id="l875">   875</a> <span class="c"># Create one instance, seeded from current time, and export its methods</span></div>
<div class="parity1 source"><a href="#l876" id="l876">   876</a> <span class="c"># as module-level functions.  The functions share state across all uses</span></div>
<div class="parity0 source"><a href="#l877" id="l877">   877</a> <span class="c">#(both in the user&#39;s code and in the Python libraries), but that&#39;s fine</span></div>
<div class="parity1 source"><a href="#l878" id="l878">   878</a> <span class="c"># for most programs and is easier for the casual user than making them</span></div>
<div class="parity0 source"><a href="#l879" id="l879">   879</a> <span class="c"># instantiate their own Random() instance.</span></div>
<div class="parity1 source"><a href="#l880" id="l880">   880</a> </div>
<div class="parity0 source"><a href="#l881" id="l881">   881</a> <span class="n">_inst</span> <span class="o">=</span> <span class="n">Random</span><span class="p">()</span></div>
<div class="parity1 source"><a href="#l882" id="l882">   882</a> <span class="n">seed</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">seed</span></div>
<div class="parity0 source"><a href="#l883" id="l883">   883</a> <span class="n">random</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">random</span></div>
<div class="parity1 source"><a href="#l884" id="l884">   884</a> <span class="n">uniform</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">uniform</span></div>
<div class="parity0 source"><a href="#l885" id="l885">   885</a> <span class="n">triangular</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">triangular</span></div>
<div class="parity1 source"><a href="#l886" id="l886">   886</a> <span class="n">randint</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">randint</span></div>
<div class="parity0 source"><a href="#l887" id="l887">   887</a> <span class="n">choice</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">choice</span></div>
<div class="parity1 source"><a href="#l888" id="l888">   888</a> <span class="n">randrange</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">randrange</span></div>
<div class="parity0 source"><a href="#l889" id="l889">   889</a> <span class="n">sample</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">sample</span></div>
<div class="parity1 source"><a href="#l890" id="l890">   890</a> <span class="n">shuffle</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">shuffle</span></div>
<div class="parity0 source"><a href="#l891" id="l891">   891</a> <span class="n">normalvariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">normalvariate</span></div>
<div class="parity1 source"><a href="#l892" id="l892">   892</a> <span class="n">lognormvariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">lognormvariate</span></div>
<div class="parity0 source"><a href="#l893" id="l893">   893</a> <span class="n">expovariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">expovariate</span></div>
<div class="parity1 source"><a href="#l894" id="l894">   894</a> <span class="n">vonmisesvariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">vonmisesvariate</span></div>
<div class="parity0 source"><a href="#l895" id="l895">   895</a> <span class="n">gammavariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">gammavariate</span></div>
<div class="parity1 source"><a href="#l896" id="l896">   896</a> <span class="n">gauss</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">gauss</span></div>
<div class="parity0 source"><a href="#l897" id="l897">   897</a> <span class="n">betavariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">betavariate</span></div>
<div class="parity1 source"><a href="#l898" id="l898">   898</a> <span class="n">paretovariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">paretovariate</span></div>
<div class="parity0 source"><a href="#l899" id="l899">   899</a> <span class="n">weibullvariate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">weibullvariate</span></div>
<div class="parity1 source"><a href="#l900" id="l900">   900</a> <span class="n">getstate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">getstate</span></div>
<div class="parity0 source"><a href="#l901" id="l901">   901</a> <span class="n">setstate</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">setstate</span></div>
<div class="parity1 source"><a href="#l902" id="l902">   902</a> <span class="n">jumpahead</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">jumpahead</span></div>
<div class="parity0 source"><a href="#l903" id="l903">   903</a> <span class="n">getrandbits</span> <span class="o">=</span> <span class="n">_inst</span><span class="o">.</span><span class="n">getrandbits</span></div>
<div class="parity1 source"><a href="#l904" id="l904">   904</a> </div>
<div class="parity0 source"><a href="#l905" id="l905">   905</a> <span class="k">if</span> <span class="n">__name__</span> <span class="o">==</span> <span class="s">&#39;__main__&#39;</span><span class="p">:</span></div>
<div class="parity1 source"><a href="#l906" id="l906">   906</a>     <span class="n">_test</span><span class="p">()</span></div>
<div class="sourcelast"></div>
</div>
</div>
</div>

<script type="text/javascript">process_dates()</script>


</body>
</html>

