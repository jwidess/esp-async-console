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

/**
 * @brief Synchronous line editing entrypoint.
 * 
 * This is the classic blocking linenoise function. It displays the prompt,
 * reads input character by character synchronously, and returns the finished line.
 * 
 * @param prompt Prompt string to display.
 * @return Heap-allocated finished string, or NULL on EOF/error. User must free it using linenoiseFree().
 */
#define linenoise async_linenoise

/**
 * @brief Add a completion suggestion string to the completions list.
 * 
 * @param completions Pointer to the completions list structure.
 * @param str Completion suggestion string.
 */
#define linenoiseAddCompletion async_linenoiseAddCompletion

/**
 * @brief Set whether empty lines are allowed to be submitted.
 * 
 * @param allow true to allow empty lines, false to ignore Enter on empty input.
 */
#define linenoiseAllowEmpty async_linenoiseAllowEmpty

/**
 * @brief Clear the terminal screen.
 */
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

/**
 * @brief Free a buffer allocated by linenoise.
 * 
 * @param ptr Pointer to the allocated memory.
 */
#define linenoiseFree async_linenoiseFree

#define linenoiseHide async_linenoiseHide

/**
 * @brief Add a line to the command history.
 * 
 * @param line String line to add.
 * @return 1 on success, 0 on error.
 */
#define linenoiseHistoryAdd async_linenoiseHistoryAdd

/**
 * @brief Free all command history entries.
 */
#define linenoiseHistoryFree async_linenoiseHistoryFree

/**
 * @brief Load history entries from a file.
 * 
 * @param filename File path to load from.
 * @return 0 on success, -1 on error.
 */
#define linenoiseHistoryLoad async_linenoiseHistoryLoad

/**
 * @brief Save history entries to a file.
 * 
 * @param filename File path to save to.
 * @return 0 on success, -1 on error.
 */
#define linenoiseHistorySave async_linenoiseHistorySave

/**
 * @brief Set the maximum command history buffer size.
 * 
 * @param len Max history capacity.
 * @return 1 on success, 0 on error.
 */
#define linenoiseHistorySetMaxLen async_linenoiseHistorySetMaxLen

#define linenoiseInsertPastedChar async_linenoiseInsertPastedChar

/**
 * @brief Get the configured terminal capability mode.
 * 
 * @return The current terminal mode setting.
 */
#define linenoiseGetTerminalMode async_linenoiseGetTerminalMode

/**
 * @brief Probe terminal columns by querying the cursor position.
 * 
 * @return Number of columns detected, or -1 on error.
 */
#define linenoiseProbe async_linenoiseProbe

/**
 * @brief Register a callback to handle tab completion.
 * 
 * @param callback The completion callback function.
 */
#define linenoiseSetCompletionCallback async_linenoiseSetCompletionCallback

/**
 * @brief Set the terminal capability mode (auto-detect, forced smart, or forced dumb).
 * 
 * @param mode The terminal capability mode.
 */
#define linenoiseSetTerminalMode async_linenoiseSetTerminalMode

/**
 * @brief Enable or disable short notifications printed on terminal capability detection changes.
 * 
 * @param enable true to enable transition logs, false to disable.
 */
#define linenoiseSetModeMessages async_linenoiseSetModeMessages

/**
 * @brief Get whether terminal capability mode switch notifications are enabled.
 * 
 * @return true if notifications are enabled.
 */
#define linenoiseGetModeMessages async_linenoiseGetModeMessages

/**
 * @brief Register a callback to free resources allocated for inline hints.
 * 
 * @param callback The free hints callback function.
 */
#define linenoiseSetFreeHintsCallback async_linenoiseSetFreeHintsCallback

/**
 * @brief Register a callback to provide inline input hints.
 * 
 * @param callback The hints callback function.
 */
#define linenoiseSetHintsCallback async_linenoiseSetHintsCallback

/**
 * @brief Set the maximum allowable command line length.
 * 
 * @param len The maximum length in bytes.
 * @return 1 on success, 0 on error.
 */
#define linenoiseSetMaxLineLen async_linenoiseSetMaxLineLen

/**
 * @brief Set multiline mode.
 * 
 * In multiline mode, linenoise will wrap long lines to multiple terminal lines
 * instead of scrolling horizontally.
 * 
 * @param ml 1 to enable multiline mode, 0 to disable.
 */
#define linenoiseSetMultiLine async_linenoiseSetMultiLine

/**
 * @brief Set terminal attributes / read characteristics (e.g. non-blocking).
 */
#define linenoiseSetReadCharacteristics async_linenoiseSetReadCharacteristics

/**
 * @brief Override the standard read function for linenoise input.
 * 
 * @param read_fn Pointer to the custom read function.
 */
#define linenoiseSetReadFunction async_linenoiseSetReadFunction

#define linenoiseShow async_linenoiseShow
#define refreshShowHints async_linenoise_refreshShowHints
#define getMillis async_linenoise_getMillis

#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Terminal Capability Modes */
typedef enum {
    LINENOISE_MODE_AUTO = 0,
    LINENOISE_MODE_SMART,
    LINENOISE_MODE_DUMB
} linenoiseTerminalMode_t;

typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
typedef const char*(linenoiseHintsCallback)(const char *, int *color, int *bold);
typedef void(linenoiseFreeHintsCallback)(void *);

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *callback);
void linenoiseSetHintsCallback(linenoiseHintsCallback *callback);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *callback);
void linenoiseAddCompletion(linenoiseCompletions *completions, const char *str);

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
void linenoiseSetTerminalMode(linenoiseTerminalMode_t mode);
linenoiseTerminalMode_t linenoiseGetTerminalMode(void);
void linenoiseSetModeMessages(bool enable);
bool linenoiseGetModeMessages(void);
void linenoiseAllowEmpty(bool allow);
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
