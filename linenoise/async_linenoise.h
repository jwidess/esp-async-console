/* linenoise.h -- VERSION 1.0
 *
 * Guerrilla line editing library against the idea that a line editing lib
 * needs to be 20,000 lines of C code.
 *
 * See linenoise.c for more information.
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (c) 2010-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LINENOISE_H
#define __LINENOISE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ── Rename all linenoise symbols to avoid ESP-IDF linker collisions ───── */
#define getMillis async_linenoise_getMillis
#define linenoise async_linenoise
#define linenoiseAddCompletion async_linenoiseAddCompletion
#define linenoiseAllowEmpty async_linenoiseAllowEmpty
#define linenoiseClearScreen async_linenoiseClearScreen
#define linenoiseEditBackspace async_linenoiseEditBackspace
#define linenoiseEditDelete async_linenoiseEditDelete
#define linenoiseEditDeletePrevWord async_linenoiseEditDeletePrevWord
#define linenoiseEditFeed async_linenoiseEditFeed
#define linenoiseEditHistoryNext async_linenoiseEditHistoryNext
#define linenoiseEditInsert async_linenoiseEditInsert
#define linenoiseEditMoveEnd async_linenoiseEditMoveEnd
#define linenoiseEditMoveHome async_linenoiseEditMoveHome
#define linenoiseEditMoveLeft async_linenoiseEditMoveLeft
#define linenoiseEditMoveRight async_linenoiseEditMoveRight
#define linenoiseEditStart async_linenoiseEditStart
#define linenoiseEditStop async_linenoiseEditStop
#define linenoiseFree async_linenoiseFree
#define linenoiseHide async_linenoiseHide
#define linenoiseHistoryAdd async_linenoiseHistoryAdd
#define linenoiseHistoryFree async_linenoiseHistoryFree
#define linenoiseHistoryLoad async_linenoiseHistoryLoad
#define linenoiseHistorySave async_linenoiseHistorySave
#define linenoiseHistorySetMaxLen async_linenoiseHistorySetMaxLen
#define linenoiseInsertPastedChar async_linenoiseInsertPastedChar
#define linenoiseIsDumbMode async_linenoiseIsDumbMode
#define linenoiseProbe async_linenoiseProbe
#define linenoiseSetCompletionCallback async_linenoiseSetCompletionCallback
#define linenoiseSetDumbMode async_linenoiseSetDumbMode
#define linenoiseSetFreeHintsCallback async_linenoiseSetFreeHintsCallback
#define linenoiseSetHintsCallback async_linenoiseSetHintsCallback
#define linenoiseSetMaxLineLen async_linenoiseSetMaxLineLen
#define linenoiseSetMultiLine async_linenoiseSetMultiLine
#define linenoiseSetReadCharacteristics async_linenoiseSetReadCharacteristics
#define linenoiseSetReadFunction async_linenoiseSetReadFunction
#define linenoiseShow async_linenoiseShow
#define refreshShowHints async_linenoise_refreshShowHints

#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
typedef const char*(linenoiseHintsCallback)(const char *, int *color, int *bold);
typedef void(linenoiseFreeHintsCallback)(void *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseSetHintsCallback(linenoiseHintsCallback *);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char *);

int linenoiseProbe(void);
char *linenoise(const char *prompt);
void linenoiseFree(void *ptr);
int linenoiseHistoryAdd(const char *line);
int linenoiseHistorySetMaxLen(int len);
int linenoiseHistorySave(const char *filename);
int linenoiseHistoryLoad(const char *filename);
void linenoiseHistoryFree(void);
void linenoiseClearScreen(void);
void linenoiseSetMultiLine(int ml);
void linenoiseSetDumbMode(int set);
bool linenoiseIsDumbMode(void);
void linenoiseAllowEmpty(bool);
int linenoiseSetMaxLineLen(size_t len);

typedef ssize_t (*linenoise_read_bytes_fn)(int, void*, size_t);
void linenoiseSetReadFunction(linenoise_read_bytes_fn read_fn);
void linenoiseSetReadCharacteristics(void);

/* ── Async API additions (project-local patch, IDF 5.5.0 base) ────────────
 *
 * linenoiseState is moved here from linenoise.c so that external modules
 * (console_io.h) can declare / define instances of the struct without
 * depending on linenoise.c internals.
 *
 * Upstream reference: antirez/linenoise linenoiseEditStart / Feed / Stop
 * ──────────────────────────────────────────────────────────────────────── */

struct linenoiseState {
    char       *buf;           /* Edited line buffer.                        */
    size_t      buflen;        /* Edited line buffer size.                   */
    const char *prompt;        /* Prompt to display.                         */
    size_t      plen;          /* Prompt length (visible chars, no escapes). */
    size_t      pos;           /* Current cursor position.                   */
    size_t      oldpos;        /* Previous refresh cursor position.          */
    size_t      len;           /* Current edited line length.                */
    size_t      cols;          /* Number of columns in terminal.             */
    size_t      maxrows;       /* Max rows used so far (multiline mode).     */
    int         history_index; /* History entry currently being edited.      */
};

/**
 * Erase the prompt and current input line from the terminal so that async
 * output (e.g. an ESP_LOG message) can be printed cleanly above it.
 * No-op when the editing state is inactive (l->buf == NULL).
 */
void linenoiseHide(struct linenoiseState *l);

/**
 * Redraw the prompt and the user's partially-typed input after async output
 * has been printed.
 * No-op when the editing state is inactive (l->buf == NULL).
 */
void linenoiseShow(struct linenoiseState *l);

/**
 * Begin a new non-blocking edit session.
 *
 * Initialises *l, writes the prompt, adds an empty history slot, and puts
 * stdin into O_NONBLOCK mode.  buf must remain valid until linenoiseEditStop.
 *
 * @return 0 on success, -1 on error.
 */
int linenoiseEditStart(struct linenoiseState *l, char *buf, size_t buflen,
                       const char *prompt);

/**
 * Feed one character's worth of input to the editor.
 *
 * Call repeatedly (e.g. every 1 ms from a FreeRTOS task) until a value other
 * than linenoiseEditMore is returned.
 *
 * @return linenoiseEditMore  No complete line yet — call again.
 * @return NULL               CTRL+C / CTRL+D / error — call linenoiseEditStop.
 * @return other char *       Heap-allocated completed line — call
 *                            linenoiseEditStop, then linenoiseFree.
 */
char *linenoiseEditFeed(struct linenoiseState *l);

/** Sentinel returned by linenoiseEditFeed when no complete line is ready. */
#define linenoiseEditMore ((char *)-1)

/**
 * End the current edit session.
 *
 * Restores stdin to blocking mode, prints a newline, and marks the state
 * inactive (l->buf = NULL) so that linenoiseHide/Show become no-ops until
 * the next linenoiseEditStart.
 */
void linenoiseEditStop(struct linenoiseState *l);

#ifdef __cplusplus
}
#endif

#endif /* __LINENOISE_H */
