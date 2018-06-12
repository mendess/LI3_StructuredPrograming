package engine;

import java.util.Comparator;

/**
 * Comparador da data dos posts.
 */
class PostDateComparator implements Comparator<Post> {


    /**
     * {@inheritDoc}
     */
    @Override
    public int compare(Post post1, Post post2){
        return post1.getDate().compareTo(post2.getDate());
    }
}
