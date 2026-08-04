//
// Unit test program for TTF library
//
// https://www.msweet.org/ttf
//
// Copyright © 2018-2026 by Michael R Sweet.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//
// Usage:
//
//   ./testttf [--list] [FILENAME]
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include "ttf.h"
#include "test.h"


//
// Local functions...
//

static void	error_cb(void *data, const char *message);
static char	*format_name(char *buffer, size_t bufsize, const char *family, ttf_style_t fstyle, ttf_weight_t fweight, ttf_stretch_t fstretch);
static int	list_fonts(bool verbose);
static int	test_find_font(ttf_cache_t *cache, const char *family, ttf_style_t fstyle, ttf_weight_t fweight, ttf_stretch_t fstretch);
static int	test_font(const char *filename, ttf_t *font);


//
// 'main()' - Main entry for unit tests.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line arguments
     char *argv[])			// I - Command-line arguments
{
  int		i;			// Looping var
  int		errors = 0;		// Number of errors
  bool		verbose = false;	// Be verbose?


  if (argc > 1)
  {
    for (i = 1; i < argc; i ++)
    {
      if (!strcmp(argv[i], "--list"))
        errors += list_fonts(verbose);
      else if (!strcmp(argv[i], "--verbose"))
        verbose = true;
      else
	errors += test_font(argv[i], /*font*/NULL);
    }
  }
  else
  {
    // Test with the bundled TrueType files...
    errors += test_font("testfiles/OpenSans-Bold.ttf", /*font*/NULL);
    errors += test_font("testfiles/OpenSans-Regular.ttf", /*font*/NULL);
    errors += test_font("testfiles/NotoSansJP-Regular.otf", /*font*/NULL);

    errors += list_fonts(false);
  }

  return (errors);
}


//
// 'error_cb()' - Error callback.
//

static void
error_cb(void       *data,		// I - User data (not used)
         const char *message)		// I - Message string
{
  testEndMessage(false, "%s", message);
}


//
// 'format_name()' - Format a font name.
//

static char *				// O - Formatted font name
format_name(char          *buffer,	// I - String buffer
            size_t        bufsize,	// I - Size of string buffer
            const char    *family,	// I - Font family
            ttf_style_t   fstyle,	// I - Font style
            ttf_weight_t  fweight,	// I - Font weight
            ttf_stretch_t fstretch)	// I - Font stretch
{
  int	weight;				// Weight index
  static const char * const stretches[] =
  {					// Font stretch values
    " <AnyStretch>",			// TTF_STRETCH_UNSPEC
    "",					// TTF_STRETCH_NORMAL
    " Ultra-Condensed",			// TTF_STRETCH_ULTRA_CONDENSED
    " Extra-Condensed",			// TTF_STRETCH_EXTRA_CONDENSED
    " Condensed",			// TTF_STRETCH_CONDENSED
    " Semi-Condensed",			// TTF_STRETCH_SEMI_CONDENSED
    " Semi-Expanded",			// TTF_STRETCH_SEMI_EXPANDED
    " Expanded",			// TTF_STRETCH_EXPANDED
    " Extra-Expanded",			// TTF_STRETCH_EXTRA_EXPANDED
    " Ultra-Expanded"			// TTF_STRETCH_ULTRA_EXPANDED
  };
  static const char * const styles[] =	// Font styles
  {
    " <AnyStyle>",			// TTF_STYLE_UNSPEC
    "",					// TTF_STYLE_NORMAL
    " Italic",				// TTF_STYLE_ITALIC
    " Oblique"				// TTF_STYLE_OBLIQUE
  };
  static const char * const weights[] =	// Font weights
  {
    " <AnyWeight>",			// TTF_WEIGHT_UNSPEC
    " Thin",				// TTF_WEIGHT_100
    " Extra-Light",			// TTF_WEIGHT_200
    " Light",				// TTF_WEIGHT_300
    "",					// TTF_WEIGHT_400
    " Medium",				// TTF_WEIGHT_500
    " Semi-Bold",			// TTF_WEIGHT_600
    " Bold",				// TTF_WEIGHT_700
    " Extra-Bold",			// TTF_WEIGHT_800
    " Black"				// TTF_WEIGHT_900
  };


  if (fweight < TTF_WEIGHT_100)
    weight = 0;
  else if (fweight > TTF_WEIGHT_900)
    weight = 9;
  else
    weight = (int)fweight / 100;

  snprintf(buffer, bufsize, "%s%s%s%s", family, stretches[fstretch - TTF_STRETCH_UNSPEC], weights[weight], styles[fstyle - TTF_STYLE_UNSPEC]);

  return (buffer);
}


//
// 'list_fonts()' - List available fonts.
//

static int				// O - Number of errors
list_fonts(bool verbose)		// I - Be verbose?
{
  ttf_cache_t	*cache;			// Font cache
  ttf_cache_t	*matches;		// Search results
  size_t	i,			// Looping var
		num_fonts,		// Number of fonts
		num_matches;		// Number of matching fonts
  time_t	start,			// Start time
		end;			// End time
  char		name[256];		// Font name
  int		errors = 0;		// Number of errors
  ttf_class_t	family_class;		// Font family class
  bool		has_sans = false,	// Have a sans-serif font?
		has_serif = false;	// Have a serif font?
  const char	*test_mono = NULL,	// Test monospaced family name...
		*test_sans = NULL,	// Test sans-serif family name...
		*test_serif = NULL;	// Test serif family name


  start = time(NULL);
  testBegin("ttfCacheCreate");
  if ((cache = ttfCacheCreate("testttf", error_cb, /*err_cbdata*/NULL)) == NULL)
    return (1);
  end = time(NULL);

  num_fonts = ttfCacheGetNumFonts(cache);

  testEndMessage(true, "%lu fonts found, %d seconds", (unsigned long)num_fonts, (int)(end - start));

  for (i = 0; i < num_fonts; i ++)
  {
    const char *family = ttfCacheGetFamily(cache, i);
					// Family name

    if (!test_mono)
    {
      if (!strcasecmp(family, "Courier"))
        test_mono = "Courier";
      else if (!strcasecmp(family, "Courier New"))
        test_mono = "Courier New";
      else if (!strcasecmp(family, "DejaVu Sans Mono"))
        test_mono = "DejaVu Sans Mono";
    }

    if (!test_sans)
    {
      if (!strcasecmp(family, "Arial"))
        test_sans = "Arial";
      else if (!strcasecmp(family, "DejaVu Sans"))
        test_sans = "DejaVu Sans";
      else if (!strcasecmp(family, "Helvetica"))
        test_sans = "Helvetica";
      else if (!strcasecmp(family, "Helvetica Neue"))
        test_sans = "Helvetica Neue";
    }

    if (!test_serif)
    {
      if (!strcasecmp(family, "DejaVu Serif"))
        test_serif = "DejaVu Serif";
      else if (!strcasecmp(family, "Times Roman"))
        test_serif = "Times Roman";
      else if (!strcasecmp(family, "Times New Roman"))
        test_serif = "Times New Roman";
    }

    family_class = ttfCacheGetFamilyClass(cache, i);

    if (family_class & TTF_CLASS_SANS_SERIF)
    {
      has_sans = true;

      if (!test_sans)
        test_sans = family;
    }

    if (family_class & (TTF_CLASS_OLDSTYLE_SERIFS | TTF_CLASS_TRANSITIONAL_SERIFS | TTF_CLASS_MODERN_SERIFS | TTF_CLASS_CLARENDON_SERIFS | TTF_CLASS_SLAB_SERIFS | TTF_CLASS_FREEFORM_SERIFS))
    {
      has_serif = true;

      if (!test_serif)
        test_serif = family;
    }

    format_name(name, sizeof(name), family, ttfCacheGetStyle(cache, i), ttfCacheGetWeight(cache, i), ttfCacheGetStretch(cache, i));

    if (!verbose)
      testMessage("    %s", name);
    else if (ttfCacheGetIndex(cache, i) > 0)
      testMessage("    %s(%u): %s", ttfCacheGetFilename(cache, i), (unsigned)ttfCacheGetIndex(cache, i), name);
    else
      testMessage("    %s: %s", ttfCacheGetFilename(cache, i), name);
  }

  if (test_mono)
  {
    errors += test_find_font(cache, test_mono, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC);
    errors += test_find_font(cache, test_mono, TTF_STYLE_NORMAL, TTF_WEIGHT_700, TTF_STRETCH_UNSPEC);
  }

  if (test_sans)
  {
    errors += test_find_font(cache, test_sans, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC);
    errors += test_find_font(cache, test_sans, TTF_STYLE_NORMAL, TTF_WEIGHT_700, TTF_STRETCH_UNSPEC);
  }

  if (test_serif)
  {
    errors += test_find_font(cache, test_serif, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC);
    errors += test_find_font(cache, test_serif, TTF_STYLE_NORMAL, TTF_WEIGHT_700, TTF_STRETCH_UNSPEC);
  }

  if (num_fonts > 0)
  {
    testBegin("ttfCacheSearch(TTF_CLASS_UNSPEC)");
    if ((matches = ttfCacheSearch(cache, TTF_CLASS_UNSPEC, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC, /*fixed_pitch*/false)) != NULL && (num_matches = ttfCacheGetNumFonts(matches)) > 0)
    {
      testEndMessage(true, "%lu fonts", (unsigned long)num_matches);

      for (i = 0; i < num_matches; i ++)
      {
	format_name(name, sizeof(name), ttfCacheGetFamily(matches, i), ttfCacheGetStyle(matches, i), ttfCacheGetWeight(matches, i), ttfCacheGetStretch(matches, i));

	if (!verbose)
	  testMessage("    %s", name);
	else if (ttfCacheGetIndex(matches, i) > 0)
	  testMessage("    %s(%u): %s", ttfCacheGetFilename(matches, i), (unsigned)ttfCacheGetIndex(matches, i), name);
	else
	  testMessage("    %s: %s", ttfCacheGetFilename(matches, i), name);
      }
    }
    else
    {
      testEnd(false);
      errors ++;
    }

    ttfCacheDelete(matches);
  }

  if (has_sans)
  {
    testBegin("ttfCacheSearch(TTF_CLASS_SANS_SERIF)");
    if ((matches = ttfCacheSearch(cache, TTF_CLASS_SANS_SERIF, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC, /*fixed_pitch*/false)) != NULL && (num_matches = ttfCacheGetNumFonts(matches)) > 0 && (ttfCacheGetFamilyClass(matches, 0) & TTF_CLASS_SANS_SERIF))
    {
      testEndMessage(true, "%lu fonts", (unsigned long)num_matches);

      for (i = 0; i < num_matches; i ++)
      {
	format_name(name, sizeof(name), ttfCacheGetFamily(matches, i), ttfCacheGetStyle(matches, i), ttfCacheGetWeight(matches, i), ttfCacheGetStretch(matches, i));

	if (!verbose)
	  testMessage("    %s", name);
	else if (ttfCacheGetIndex(matches, i) > 0)
	  testMessage("    %s(%u): %s", ttfCacheGetFilename(matches, i), (unsigned)ttfCacheGetIndex(matches, i), name);
	else
	  testMessage("    %s: %s", ttfCacheGetFilename(matches, i), name);
      }
    }
    else
    {
      testEnd(false);
      errors ++;
    }

    ttfCacheDelete(matches);
  }

  if (has_serif)
  {
    static const ttf_class_t serif_classes = TTF_CLASS_OLDSTYLE_SERIFS | TTF_CLASS_TRANSITIONAL_SERIFS | TTF_CLASS_MODERN_SERIFS | TTF_CLASS_CLARENDON_SERIFS | TTF_CLASS_SLAB_SERIFS | TTF_CLASS_FREEFORM_SERIFS;

    testBegin("ttfCacheSearch(serif classes)");
    if ((matches = ttfCacheSearch(cache, serif_classes, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC, /*fixed_pitch*/false)) != NULL && (num_matches = ttfCacheGetNumFonts(matches)) > 0 && (ttfCacheGetFamilyClass(matches, 0) & serif_classes))
    {
      testEndMessage(true, "%lu fonts", (unsigned long)num_matches);

      for (i = 0; i < num_matches; i ++)
      {
	format_name(name, sizeof(name), ttfCacheGetFamily(matches, i), ttfCacheGetStyle(matches, i), ttfCacheGetWeight(matches, i), ttfCacheGetStretch(matches, i));

	if (!verbose)
	  testMessage("    %s", name);
	else if (ttfCacheGetIndex(matches, i) > 0)
	  testMessage("    %s(%u): %s", ttfCacheGetFilename(matches, i), (unsigned)ttfCacheGetIndex(matches, i), name);
	else
	  testMessage("    %s: %s", ttfCacheGetFilename(matches, i), name);
      }
    }
    else
    {
      testEnd(false);
      errors ++;
    }

    ttfCacheDelete(matches);
  }

  if (test_mono)
  {
    testBegin("ttfCacheSearch(fixed-pitch)");
    if ((matches = ttfCacheSearch(cache, TTF_CLASS_UNSPEC, TTF_STYLE_UNSPEC, TTF_WEIGHT_UNSPEC, TTF_STRETCH_UNSPEC, /*fixed_pitch*/true)) != NULL && (num_matches = ttfCacheGetNumFonts(matches)) > 0)
    {
      bool all_mono = true;		// Are all matching fonts fixed pitch?

      for (i = 0; i < num_matches; i ++)
      {
        if (!ttfCacheIsFixedPitch(matches, i))
        {
          all_mono = false;
          break;
        }
      }

      if (all_mono)
      {
	testEndMessage(true, "%lu fonts", (unsigned long)num_matches);

	for (i = 0; i < num_matches; i ++)
	{
	  format_name(name, sizeof(name), ttfCacheGetFamily(matches, i), ttfCacheGetStyle(matches, i), ttfCacheGetWeight(matches, i), ttfCacheGetStretch(matches, i));

	  if (!verbose)
	    testMessage("    %s", name);
	  else if (ttfCacheGetIndex(matches, i) > 0)
	    testMessage("    %s(%u): %s", ttfCacheGetFilename(matches, i), (unsigned)ttfCacheGetIndex(matches, i), name);
	  else
	    testMessage("    %s: %s", ttfCacheGetFilename(matches, i), name);
	}
      }
      else
      {
        testEnd(false);
        errors ++;
      }
    }
    else
    {
      testEnd(false);
      errors ++;
    }

    ttfCacheDelete(matches);
  }

  testBegin("ttfCacheIsFixedPitch");
  {
    bool	all_match = true;	// All cached flags match the fonts?

    for (i = 0; i < num_fonts; i ++)
    {
      ttf_t *cfont = ttfCacheGetFont(cache, i);

      if (cfont && ttfCacheIsFixedPitch(cache, i) != ttfIsFixedPitch(cfont))
      {
        all_match = false;
        break;
      }
    }

    if (all_match)
      testEndMessage(true, "%lu fonts", (unsigned long)num_fonts);
    else
    {
      testEnd(false);
      errors ++;
    }
  }

  return (errors);
}


//
// 'test_find_font()' - Test finding a font.
//

static int				// O - Number of errors
test_find_font(ttf_cache_t   *cache,	// I - Font cache
	       const char    *family,	// I - Font family
	       ttf_style_t   fstyle,	// I - Font style
	       ttf_weight_t  fweight,	// I - Font weight
	       ttf_stretch_t fstretch)	// I - Font stretch
{
  ttf_t	*font;				// Matching font
  char	name[256];			// Font name


  testBegin("ttfCacheFind(%s)", format_name(name, sizeof(name), family, fstyle, fweight, fstretch));
  if ((font = ttfCacheFind(cache, family, fstyle, fweight, fstretch)) != NULL)
  {
    testEndMessage(true, "%s", format_name(name, sizeof(name), ttfGetFamily(font), ttfGetStyle(font), ttfGetWeight(font), ttfGetStretch(font)));

    return (test_font(ttfGetFilename(font), font));
  }
  else
  {
    testEnd(false);
    return (1);
  }
}


//
// 'test_font()' - Test a font file.
//

static int				// O - Number of errors
test_font(const char *filename,		// I - Font filename or `NULL`
          ttf_t      *font)		// I - Font or `NULL` to load
{
  int		i,			// Looping var
		errors = 0;		// Number of errors
  struct stat	fileinfo;		// Font file information
  FILE		*fp = NULL;		// File pointer
  void		*data = NULL;		// Memory buffer for font file
  const char	*value;			// Font (string) value
  int		intvalue;		// Font (integer) value
  float		realvalue;		// Font (real) value
  char		psname[1024];		// Postscript font name
  ttf_rect_t	bounds;			// Bounds
  ttf_rect_t	extents;		// Extents
  size_t	j,			// Looping var
		num_adjs;		// Number of kerning adjustments
  double	adjs[1024];		// Kerning adjustments
  size_t	num_fonts;		// Number of fonts
  ttf_style_t	style;			// Font style
  ttf_weight_t	weight;			// Font weight
  ttf_class_t	family_class;		// Font family class
  bool		reload;			// Reload the font from memory?
  const int	*cmap;			// CMap table
  size_t	num_cmap;		// Number of CMap entries
  static const char * const stretches[] =
  {					// Font stretch strings
    "TTF_STRETCH_NORMAL",		// normal
    "TTF_STRETCH_ULTRA_CONDENSED",	// ultra-condensed
    "TTF_STRETCH_EXTRA_CONDENSED",	// extra-condensed
    "TTF_STRETCH_CONDENSED",		// condensed
    "TTF_STRETCH_SEMI_CONDENSED",	// semi-condensed
    "TTF_STRETCH_SEMI_EXPANDED",	// semi-expanded
    "TTF_STRETCH_EXPANDED",		// expanded
    "TTF_STRETCH_EXTRA_EXPANDED",	// extra-expanded
    "TTF_STRETCH_ULTRA_EXPANDED"	// ultra-expanded
  };
  static const char * const strings[] =	// Test strings
  {
    "Hello, World!",			// English
    "مرحبا بالعالم!",			// Arabic
    "Bonjour le monde!",		// French
    "Γειά σου Κόσμε!",			// Greek
    "שלום עולם!",			// Hebrew
    "Привет мир!",			// Russian
    "こんにちは世界！"			// Japanese
  };
  static const char * const styles[] =	// Font style names
  {
    "TTF_STYLE_NORMAL",
    "TTF_STYLE_ITALIC",
    "TTF_STYLE_OBLIQUE"
  };


  reload = (filename != NULL && font == NULL);

  if (filename && !font)
  {
    testBegin("ttfCreate(\"%s\")", filename);
    if ((font = ttfCreate(filename, 0, error_cb, NULL)) != NULL)
      testEnd(true);
    else
      return (1);
  }

  testBegin("ttfGetFilename");
  value = ttfGetFilename(font);
  if ((value != NULL) == (filename != NULL))
    testEnd(true);
  else if (value)
    testEndMessage(false, "Got filename \"%s\" instead of NULL.", value);
  else
    testEndMessage(false, "Got NULL instead of filename \"%s\".", filename);

  testBegin("ttfContainsChar(' ')");
  testEnd(ttfContainsChar(font, ' '));

  testBegin("ttfContainsChar('\\177')");
  testEnd(!ttfContainsChar(font, '\177'));

  testBegin("ttfContainsChars(\"Hello, World!\")");
  testEnd(ttfContainsChars(font, "Hello, World!"));

  testBegin("ttfGetAscent");
  if ((intvalue = ttfGetAscent(font)) > 0)
  {
    testEndMessage(true, "%d", intvalue);
  }
  else
  {
    testEndMessage(false, "%d", intvalue);
    errors ++;
  }

  testBegin("ttfGetBounds");
  if (ttfGetBounds(font, &bounds))
  {
    testEndMessage(true, "%g %g %g %g", bounds.left, bounds.bottom, bounds.right, bounds.top);
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetCapHeight");
  if ((intvalue = ttfGetCapHeight(font)) > 0)
  {
    testEndMessage(true, "%d", intvalue);
  }
  else
  {
    testEndMessage(false, "%d", intvalue);
    errors ++;
  }

  testBegin("ttfGetCMap");
  if ((cmap = ttfGetCMap(font, &num_cmap)) != NULL && num_cmap > 0)
  {
    testEndMessage(true, "%lu entries", (unsigned long)num_cmap);
  }
  else
  {
    testEndMessage(false, "%lu entries", (unsigned long)num_cmap);
    errors ++;
  }

  testBegin("ttfGetCopyright");
  if ((value = ttfGetCopyright(font)) != NULL)
  {
    testEndMessage(true, "%s", value);
  }
  else
  {
    testEndMessage(true, "warning: no copyright found");
  }

  for (i = 0; i < (int)(sizeof(strings) / sizeof(strings[0])); i ++)
  {
    testBegin("ttfGetExtents(\"%s\")", strings[i]);
    if (ttfGetExtents(font, 12.0f, strings[i], &extents))
    {
      testEndMessage(true, "%.1f %.1f %.1f %.1f", extents.left, extents.bottom, extents.right, extents.top);
    }
    else
    {
      testEnd(false);
      errors ++;
    }

    testBegin("ttfGetKernedExtents(\"%s\")", strings[i]);
    if ((num_adjs = ttfGetKernedExtents(font, 12.0f, strings[i], &extents, sizeof(adjs) / sizeof(adjs[0]), adjs)) > 0)
    {
      testEndMessage(true, "%.1f %.1f %.1f %.1f, num_adjs=%u", extents.left, extents.bottom, extents.right, extents.top, (unsigned)num_adjs);
      for (j = 0; j < num_adjs; j ++)
      {
        if (adjs[j] != 0.0)
          testMessage("    adjs[%u]=%.3f", (unsigned)j, adjs[j]);
      }
    }
    else
    {
      testEnd(false);
      errors ++;
    }
  }

  testBegin("ttfGetFamily");
  if ((value = ttfGetFamily(font)) != NULL)
  {
    testEndMessage(true, "%s", value);
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetItalicAngle");
  if ((realvalue = ttfGetItalicAngle(font)) >= -180.0 && realvalue <= 180.0)
  {
    testEndMessage(true, "%g", realvalue);
  }
  else
  {
    testEndMessage(false, "%g", realvalue);
    errors ++;
  }

  testBegin("ttfGetNumFonts");
  if ((num_fonts = ttfGetNumFonts(font)) > 0)
  {
    testEndMessage(true, "%u", (unsigned)num_fonts);
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetPostScriptName");
  if ((value = ttfGetPostScriptName(font)) != NULL)
  {
    testEndMessage(true, "%s", value);

    strncpy(psname, value, sizeof(psname) - 1);
    psname[sizeof(psname) - 1] = '\0';
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetStretch");
  if ((intvalue = (int)ttfGetStretch(font)) >= TTF_STRETCH_NORMAL && intvalue <= TTF_STRETCH_ULTRA_EXPANDED)
  {
    testEndMessage(true, "%s", stretches[intvalue]);
  }
  else
  {
    testEndMessage(false, "%d", intvalue);
    errors ++;
  }

  testBegin("ttfGetStyle");
  if ((style = ttfGetStyle(font)) >= TTF_STYLE_NORMAL && style <= TTF_STYLE_OBLIQUE)
  {
    testEndMessage(true, "%s", styles[style]);
  }
  else
  {
    testEndMessage(false, "Unknown/%d", style);
    errors ++;
  }

  testBegin("ttfGetVersion");
  if ((value = ttfGetVersion(font)) != NULL)
  {
    testEndMessage(true, "%s", value);
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetWeight");
  if ((weight = ttfGetWeight(font)) >= 0)
  {
    testEndMessage(true, "%u", (unsigned)weight);
  }
  else
  {
    testEnd(false);
    errors ++;
  }

  testBegin("ttfGetFamilyClass");
  if (!((family_class = ttfGetFamilyClass(font)) & ~(ttf_class_t)0x07ff))
  {
    testEndMessage(true, "0x%04x", (unsigned)family_class);
  }
  else
  {
    testEndMessage(false, "0x%04x", (unsigned)family_class);
    errors ++;
  }

  testBegin("ttfGetWidth(' ')");

  if ((intvalue = ttfGetWidth(font, ' ')) > 0)
  {
    testEndMessage(true, "%d", intvalue);
  }
  else
  {
    testEndMessage(false, "%d", intvalue);
    errors ++;
  }

  testBegin("ttfGetXHeight");
  if ((intvalue = ttfGetXHeight(font)) > 0)
  {
    testEndMessage(true, "%d", intvalue);
  }
  else
  {
    testEndMessage(false, "%d", intvalue);
    errors ++;
  }

  testBegin("ttfIsFixedPitch");
  testEndMessage(true, "%s", ttfIsFixedPitch(font) ? "true" : "false");

  // Return immediately if we didn't create the font here (e.g., it was loaded
  // from the font cache and is managed by the caller)...
  if (!reload)
    return (errors);

  // Delete the first font and try getting it from memory...
  ttfDelete(font);
  font = NULL;

  // Now copy the font to memory and open it that way...
  testBegin("fopen(\"%s\", \"rb\")", filename);
  if ((fp = fopen(filename, "rb")) == NULL)
  {
    testEndMessage(false, "%s", strerror(errno));
    errors ++;
  }
  else
  {
    testEndMessage(true, "%d", fileno(fp));
    testBegin("fstat(%d)", fileno(fp));
    if (fstat(fileno(fp), &fileinfo))
    {
      testEndMessage(false, "%s", strerror(errno));
      errors ++;
    }
    else
    {
      testEndMessage(true, "%lu bytes", (unsigned long)fileinfo.st_size);

      testBegin("malloc(%lu)", (unsigned long)fileinfo.st_size);
      if ((data = malloc((size_t)fileinfo.st_size)) == NULL)
      {
	testEndMessage(false, "%s", strerror(errno));
	errors ++;
      }
      else
      {
	testEnd(true);
        testBegin("fread(%lu)", (unsigned long)fileinfo.st_size);
        if (fread(data, (size_t)fileinfo.st_size, 1, fp) != 1)
        {
	  testEndMessage(false, "%s", strerror(errno));
	  errors ++;
        }
        else
        {
          testEnd(true);
          testBegin("ttfCreateData()");
          if ((font = ttfCreateData(data, (size_t)fileinfo.st_size, /*idx*/0, error_cb, /*err_data*/NULL)) == NULL)
          {
            errors ++;
          }
          else
          {
            testEnd(true);

	    testBegin("ttfGetPostScriptName");
	    if ((value = ttfGetPostScriptName(font)) != NULL)
	    {
	      if (!strcmp(value, psname))
	      {
		testEndMessage(true, "%s", value);
	      }
	      else
	      {
		testEndMessage(false, "got \"%s\", expected \"%s\"", value, psname);
		errors ++;
	      }
	    }
	    else
	    {
	      testEnd(false);
	      errors ++;
	    }
          }
        }
      }
    }

    if (fp)
      fclose(fp);

    free(data);

    ttfDelete(font);
  }

  return (errors);
}
