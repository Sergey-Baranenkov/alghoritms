#!/usr/bin/env python
# coding: utf-8

# In[ ]:


from nltk import TweetTokenizer
import rulemma
import rupostagger
import re
from string import punctuation
from stop_words import get_stop_words
from collections import defaultdict
import os
import readchar

# In[2]:


print("START")
approved = ["СУЩЕСТВИТЕЛЬНОЕ", "ПРИЛАГАТЕЛЬНОЕ", "ГЛАГОЛ", "НАРЕЧИЕ", "ДЕЕПРИЧАСТИЕ"]
stopwords = get_stop_words('russian')
stopSymbols = '!"#$%&\'()*+,./:;<=>?@[\\]^_`{|}~'
totalTweets = 0
uniqWords = {}
bogoslens = defaultdict(int)
truelens = defaultdict(int)
lemmatizer = rulemma.Lemmatizer()
lemmatizer.load()

tokenizer = TweetTokenizer(strip_handles=True, reduce_len=True, preserve_case=False)

tagger = rupostagger.RuPosTagger()
tagger.load()

with open("data.txt", "r", encoding="utf-8") as data:
    for twit in data:
        if twit == "\n":
            continue
        totalTweets += 1
        twitLen = 0
        checkedLemmas = ['']
        twit = re.sub(r'http\S+|pic.twitter\S+', '', twit)
        twit = twit.translate(str.maketrans(stopSymbols, ' ' * 31))
        tokens = tokenizer.tokenize(twit)
        tags = tagger.tag(tokens)
        lemmas = lemmatizer.lemmatize(tags)
        for lemmatized in lemmas:
            word = lemmatized[2]
            partOfSpeech = lemmatized[3]
            if partOfSpeech in approved and word not in checkedLemmas and word not in stopwords and len(
                    word) > 2 and bool(re.fullmatch(r'[а-яa-z]*', word)):
                checkedLemmas.append(word)
                dictWord = uniqWords.get(word, [0, partOfSpeech])
                dictWord[0] += 1
                uniqWords[word] = dictWord
                twitLen += 1
        bogoslens[len(tokens)] += 1
        truelens[twitLen] += 1

print("READY")

# In[3]:


with open("true_twits_length.txt", "w", encoding="utf-8") as lengthsFile:
    for length, timesMet in sorted(truelens.items(), key=lambda i: i[1], reverse=True):
        lengthsFile.write("{}-{}-{}%\n".format(length, timesMet, timesMet / totalTweets))

# In[4]:


with open("bogos_twits_length.txt", "w", encoding="utf-8") as lengthsFile:
    for length, timesMet in sorted(bogoslens.items(), key=lambda i: i[1], reverse=True):
        lengthsFile.write("{}-{}-{}%\n".format(length, timesMet, timesMet / totalTweets))

# In[5]:


with open("frequencies.txt", "w", encoding="utf-8") as freqFile:
    for word, wordInfo in sorted(uniqWords.items(), key=lambda i: i[1][0], reverse=True):
        freqFile.write("{}-{}-{}%\n".format(word, wordInfo[0], wordInfo[0] / totalTweets))

fdWords = []
lehadict = {}
with open("myfirstdict.txt", "r", encoding="utf-8") as firstdict:
    for row in firstdict:
        fdWords.append(row.split()[0])
print(len(fdWords))

with open("superdict.txt", "r", encoding="utf-8") as ldict:
    for row in ldict:
        row = row.split()
        lehadict[row[0]] = row[1]

print(len(lehadict))
count = 0
with open("mydict.txt", "w", encoding="utf-8") as dictFile:
    for word in uniqWords:
        count += 1
        if word in fdWords:
            continue
        elif word in lehadict:
            access = int(lehadict[word])
            if access < -1:
                access = -1
            elif access > 1:
                access = 1
            dictFile.write("{} {}\n".format(word, access))
        else:
            print(count)
            try:
                print(word)
            except:
                print("error")
            access = readchar.readchar()
            if access == b'q':
                access = -1
            elif access == b'e':
                access = 1
            else:
                access = 0
            dictFile.write("{} {}\n".format(word, access))

print('ready')



