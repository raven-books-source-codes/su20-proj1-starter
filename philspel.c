/**
 * @file philspel.c
 * @author raven (zhang.xingrui@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-12
 * 
 * @copyright Copyright (c) 2021
 * 本程序框架看起来并没有释放hashtable. 不过whatever了
 */

/*
 * Include the provided hash table library.
 */
#include "hashtable.h"

/*
 * Include the header file.
 */
#include "philspel.h"

/*
 * Standard IO and file routines.
 */
#include <stdio.h>

/*
 * General utility routines (including malloc()).
 */
#include <stdlib.h>

/*
 * Character utility routines.
 */
#include <ctype.h>

/*
 * String utility routines.
 */
#include <string.h>

#include <assert.h>

/*
 * This hash table stores the dictionary.
 */
HashTable *dictionary;

/*
 * The MAIN routine.  You can safely print debugging information
 * to standard error (stderr) as shown and it will be ignored in
 * the grading process.
 */
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Specify a dictionary\n");
    return 0;
  }
  /*
   * Allocate a hash table to store the dictionary.
   */
  fprintf(stderr, "Creating hashtable\n");
  dictionary = createHashTable(2255, &stringHash, &stringEquals);

  fprintf(stderr, "Loading dictionary %s\n", argv[1]);
  readDictionary(argv[1]);
  fprintf(stderr, "Dictionary loaded\n");

  fprintf(stderr, "Processing stdin\n");
  processInput();

  /*
   * The MAIN function in C should always return 0 as a way of telling
   * whatever program invoked this that everything went OK.
   */
  return 0;
}

/*
 * This should hash a string to a bucket index.  Void *s can be safely cast
 * to a char * (null terminated string) and is already done for you here
 * for convenience.
 */
unsigned int stringHash(void *s) {
  char *string = (char *)s;
  // -- TODO --
  unsigned int hash = 3581;
  int c;

  while ((c = *string++)) {
    hash = (hash << 5) + hash + c; /* hash * 3 + c */
  }
  return hash;
}

/*
 * This should return a nonzero value if the two strings are identical
 * (case sensitive comparison) and 0 otherwise.
 */
int stringEquals(void *s1, void *s2) {
  char *string1 = (char *)s1;
  char *string2 = (char *)s2;
  // -- TODO --
  return !strcmp(string1, string2);
}

/*
 * This function should read in every word from the dictionary and
 * store it in the hash table.  You should first open the file specified,
 * then read the words one at a time and insert them into the dictionary.
 * Once the file is read in completely, return.  You will need to allocate
 * (using malloc()) space for each word.  As described in the spec, you
 * can initially assume that no word is longer than 60 characters.  However,
 * for the final 20% of your grade, you cannot assumed that words have a bounded
 * length.  You CANNOT assume that the specified file exists.  If the file does
 * NOT exist, you should print some message to standard error and call exit(1)
 * to cleanly exit the program.
 *
 * Since the format is one word at a time, with new lines in between,
 * you can safely use fscanf() to read in the strings until you want to handle
 * arbitrarily long dictionary chacaters.
 */
void readDictionary(char *dictName) {
  // -- TODO --
  FILE *file = fopen(dictName, "r");
  if (file == NULL) {
    perror("Open dictionary file failed");
    exit(1);
  }

  const size_t base_len = 1000;
  char *buf = (char *)malloc(base_len * sizeof(char));
  while (!feof(file)) {
    fgets(buf, base_len, file);
    assert(buf != NULL);
    size_t word_len = strlen(buf);
    // fgets will get also get the tailing \n, we need to remove it
    buf[word_len - 1] = '\0';
    void *word = malloc(word_len * sizeof(char));
    memcpy(word, buf, word_len);
    // Insert to hash table
    insertData(dictionary, word, word);
  }
  free(buf);

  fclose(file);
}

/*
 * This should process standard input (stdin) and copy it to standard
 * output (stdout) as specified in the spec (e.g., if a standard
 * dictionary was used and the string "this is a taest of  this-proGram"
 * was given to stdin, the output to stdout should be
 * "this is a teast [sic] of  this-proGram").  All words should be checked
 * against the dictionary as they are input, then with all but the first
 * letter converted to lowercase, and finally with all letters converted
 * to lowercase.  Only if all 3 cases are not in the dictionary should it
 * be reported as not found by appending " [sic]" after the error.
 *
 * Since we care about preserving whitespace and pass through all non alphabet
 * characters untouched, scanf() is probably insufficent (since it only
 * considers whitespace as breaking strings), meaning you will probably have to
 * get characters from stdin one at a time.
 *
 * Do note that even under the initial assumption that no word is longer than 60
 * characters, you may still encounter strings of non-alphabetic characters
 * (e.g., numbers and punctuation) which are longer than 60 characters. Again,
 * for the final 20% of your grade, you cannot assume words have a bounded
 * length.
 */

static int checkOneWord(char *word, int len) {
  int ret = 0;
  // Case1: Check whether the original word is in the hash table
  ret = findData(dictionary, word) != NULL;
  if (ret)
    return 1;
  // Case2: Set first letter to lowercase, and do check
  word[0] = tolower(word[0]);
  ret = findData(dictionary, word) != NULL;
  if (ret)
    return 1;
  // Case3: Set all letters to lowercase, and do check
  for (int i = 1; i < len; i++) {
    word[i] = tolower(word[i]);
  }
  ret = findData(dictionary, word) != NULL;
  if (ret)
    return 1;
  return 0;
}

void processInput() {
  // -- TODO --
  int word_len = 120;
  char *word = (char *)malloc(word_len * sizeof(char));
  int cur_len = 0;
  char c;

  while ((c = fgetc(stdin)) != EOF) {
    if (isalpha(c)) {
      if (cur_len == word_len) {
        word_len <<= 1;
        char *new_word = realloc(word, word_len);
        free(word);
        word = new_word;
      }
      word[cur_len++] = c;
    } else {
      // First check the existing word need to put.
      if (cur_len != 0) {
        word[cur_len] = '\0'; // Add tailer terminator
        fputs(word, stdout);
        if (!checkOneWord(word, cur_len)) {
          fputs(" [sic]", stdout);
        }
        cur_len = 0;
      }
      // Then just print current character
      fputc(c, stdout);
    }
  }

  // Print the last word
  if (cur_len != 0) {
    word[cur_len] = '\0'; // Add tailer terminator
    fputs(word, stdout);
    if (!checkOneWord(word, cur_len)) {
      fputs(" [sic]", stdout);
    }
    cur_len = 0;
  }

  // Free resource
  free(word);
  // TODO: free hash table nodes.
}
