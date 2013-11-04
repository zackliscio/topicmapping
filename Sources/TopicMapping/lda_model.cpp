

// values below this threshold are 
// not included in betas
# define SPARSE_limit 1e-12



void word_corpus::set_class_words_to_zeros_map() {
    
    // initializing class_word_ldav_ and class_total_ldav_
    // class_word_ldav_map_[wn][topic] 
    // class_total_ldav_map_[topic]
    
    // class_word_ldav_map_ should be already initialized
    assert_ints(word_occurrences_.size(), class_word_ldav_map_.size());
    RANGE_loop(wn, class_word_ldav_map_) {
        class_word_ldav_map_[wn].clear();
    }
    class_total_ldav_map_.clear();
    
}





void word_corpus::initialize_lda_data(deque<mapid> & doc_topic, 
                                      map<int, mapid> & topic_word) {
    
    // this function is getting all data structure for lda topics ready
    // some asserts are also done and should be removed later
    
    // ---------- TEMPORARY ----------
    /*
    topic_word.clear();
    ifstream gin("betas.txt");
    //ifstream gin("run_exp/final.beta");
    string gins;
    int count_line=0;
    while(getline(gin, gins)) {
        DD vv;
        cast_string_to_doubles(gins, vv);
        mapid topic_word_mapid;
        double check_sum=0.;
        RANGE_loop(i, vv) {
            topic_word_mapid[i]=exp(vv[i]);
            check_sum+=topic_word_mapid[i];
        }
        if(fabs(check_sum-1)>1e-4) {
            cerr<<"error in check_sum "<<check_sum-1<<endl; exit(-1);
        }
        topic_word[count_line]=topic_word_mapid;
        count_line+=1;
    }
    // TEMPORARY ---------------------
    //*/
    
    // asserting everything is starting from zero and being consecutive
    DI all_topics;
    for (map<int, mapid>::iterator topic_itm= topic_word.begin(); 
         topic_itm!=topic_word.end(); topic_itm++) {
        all_topics.push_back(topic_itm->first);
    }
    assert_consecutive(all_topics);
    // assertation done
    
    //lda data structures
    
    num_topics_ldav_=all_topics.size();
    cout<<"number of topics for lda em: "<<num_topics_ldav_<<endl;
    // TODO 
    // the initialization of alphas_ldav_ is quite arbitrary 
    // and should be done
    // using doc_topic
    alphas_ldav_.clear();
    alphas_ldav_.assign(num_topics_ldav_, 0.1336611513);
    
    
    
    DD void_dd_numtops;
    void_dd_numtops.assign(num_topics_ldav_, 0.);

    gammas_ldav_.clear();
    // initializing gammas
    RANGE_loop(doc_number, docs_) {
        gammas_ldav_.push_back(void_dd_numtops);
    }
    
    
    cout<<"============== DIMENSIONS =============="<<endl;
    cout<<"number of topics for lda em: "<<num_topics_ldav_<<endl;
    
    //cout<<"phis_ldav_ "<<phis_ldav_.size()<<" x "<<phis_ldav_[0].size()<<endl;
    //cout<<"betas_ldav_ "<<betas_ldav_.size()<<" x "<<betas_ldav_[0].size()<<endl;
    
    cout<<"alphas_ldav_ "<<alphas_ldav_.size()<<endl;
    
    //cout<<"class_word_ldav_ "<<class_word_ldav_.size()<<" x "<<class_word_ldav_[0].size()<<endl;
    //cout<<"class_total_ldav_ "<<class_total_ldav_.size()<<endl;
    
    cout<<"gammas_ldav_ "<<gammas_ldav_.size()<<" x "<<gammas_ldav_[0].size()<<endl;
    
    // =============== sparse data structure initialization ======================= //
    
    phis_ldav_map_.clear();
    betas_ldav_map_.clear();
    class_word_ldav_map_.clear();
    class_total_ldav_map_.clear();
    gammas_ldav_map_.clear();

    
    // betas_ldav_map_, phis_ldav_map_ and class_word_ldav_map_ have all the same structure
    // betas_ldav_map_[wn]= { topic : value }
    mapid void_mapid;
    deqid void_deqid;
    RANGE_loop(wn, word_occurrences_) {        
        betas_ldav_map_.push_back(void_mapid);
        phis_ldav_map_.push_back(void_deqid);
        class_word_ldav_map_.push_back(void_mapid);
    }
    
    
    // copying topic_word in betas_ldav_
    // this should be avoided and pass betas_ldav_ directly
    // to the lda model
    // ========= this is basically topic word but written  in a 
    // different format
    // make sure it's sparse!!!!!!!!!!!!!!!!
    // make also sure that it's normalized
    // sum_wn exp(betas_ldav_map_[wn][whatever_topic]) =1
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    for (map<int, mapid>::iterator topic_itm= topic_word.begin(); 
         topic_itm!=topic_word.end(); topic_itm++) {
        IT_loop(mapid, itm2, topic_itm->second) { 
            int word_wn=itm2->first;
            assert_ints(itm2->first, word_wn);
            // words which are below this threshold should never 
            // be relevant in this topic
            if(itm2->second>SPARSE_limit)
                betas_ldav_map_[word_wn][topic_itm->first] = log(itm2->second);
        }
    }


    cout<<"check normalization"<<endl;
    mapid topic_norm;
    RANGE_loop(wn, word_occurrences_) {
        mapid & betas_ldav_wn = betas_ldav_map_.at(wn);
        IT_loop(mapid, topic_pr, betas_ldav_wn) {
            int_histogram(topic_pr->first, topic_norm, exp(topic_pr->second));
        }        
    }
    cout<<"________________"<<endl;
    prints(topic_norm);
    
    set_class_words_to_zeros_map();
    
    RANGE_loop(doc_number, docs_) {
        // we only keep track of topics
        // which actually have words in doc_number
        // for the rest:
        // gammas_ldav_map_[missing_topic]=alphas_ldav_[missing_topic]
        // but this is not recorded
        gammas_ldav_map_.push_back(void_mapid);
        IT_loop(deqii, wn_occ, docs_[doc_number].wn_occs_) {
            IT_loop(mapid, topic_prob, betas_ldav_map_[wn_occ->first]) {
                // different normalizations could be possible here
                int_histogram(topic_prob->first, gammas_ldav_map_[doc_number], 0.);
            }
        }
    }
    
    
    

    
    
    
    
}




